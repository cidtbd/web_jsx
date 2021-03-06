/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//2:46 AM 11/21/2018
#include "native_wrapper.h"
using namespace sow_web_jsx;
/*[Help]*/
std::string*_root_dir;
bool _is_cli = false;
bool _is_flush = false;
std::stringstream _body_stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
std::map<std::string, std::string>* _headers;
std::vector<std::string>* _cookies;
std::vector<std::string>* _http_status;
/*[/Help]*/
void npgsql_execute_io(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Connection string required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Stored procedure required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Authentication context required!!!")));
		return;
	}
	if (!args[3]->IsString() || args[3]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "form_data required!!!")));
		return;
	}
	npgsql* pgsql = new npgsql();
	malloc(sizeof pgsql);
	int rec = !pgsql->is_iniit ? -1 : 0;
	if (rec < 0) {
		//std::cout << "UNABLE TO LOAD PGSQL LIB==>" << pgsql->get_last_error() << "\r\n";
		free(pgsql);
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Unable to initialize npgsql instance!!!")));
		return;
	}
	v8::String::Utf8Value vcon_str(isolate, args[0]);
	rec = pgsql->connect(*vcon_str);
	if (rec < 0 || pgsql->conn_state != connection_state::OPEN) {
		//std::cout << "Unable to connect db==>" << pgsql->get_last_error() << "\r\n";
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, pgsql->get_last_error())));
		free(pgsql);
		return;
	};
	std::map<std::string, char*>* result = new std::map<std::string, char*>();
	malloc(sizeof result);
	v8::String::Utf8Value sp_str(isolate, args[1]);
	v8::String::Utf8Value ctx_str(isolate, args[2]);
	v8::String::Utf8Value form_data_str(isolate, args[3]);
	//int npgsql::execute_io(const char * sp, const char * ctx, const char * form_data, std::map<std::string, char*>& result)
	rec = pgsql->execute_io(*sp_str, *ctx_str, *form_data_str, *result);
	if (rec < 0 ) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, pgsql->get_last_error())));
		free(pgsql); free(result);
		return;
	};
	pgsql->close();
	free(pgsql);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	for (auto itr = result->begin(); itr != result->end(); ++itr) {
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, itr->first.c_str()), 
			v8::String::NewFromUtf8(isolate, const_cast<const char*>(itr->second))
		);
	}
	delete result;
	args.GetReturnValue().Set(v8_result);
	v8_result.Clear();
};
void npgsql_execute_scalar(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not implemented yet!!!")));
	return;
};
void npgsql_execute_non_query(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not implemented yet!!!")));
};
void npgsql_data_reader(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Connection string required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Query string required!!!")));
		return;
	}
	if (!args[2]->IsFunction()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Callback required!!!")));
		return;
	}
	std::list<npgsql_param_type*>* _sql_param = new std::list<npgsql_param_type*>();
	v8::Persistent<v8::Function> cb;
	
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[2]));
	npgsql* pgsql = new npgsql();
	malloc(sizeof pgsql);
	int rec = !pgsql->is_iniit ? -1 : 0;
	if (rec < 0) {
		free(pgsql);
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Unable to initialize npgsql instance!!!")));
		return;
	}
	v8::String::Utf8Value vcon_str(isolate, args[0]);
	rec = pgsql->connect(*vcon_str);
	if (rec < 0 || pgsql->conn_state != connection_state::OPEN) {
		//std::cout << "Unable to connect db==>" << pgsql->get_last_error() << "\r\n";
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, pgsql->get_last_error())));
		free(pgsql);
		return;
	};
	v8::String::Utf8Value query_str(isolate, args[1]);
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	//5:38 AM 11/21/2018
	v8::Local<v8::Object>global = args.Holder();
	rec = pgsql->execute_scalar_l(*query_str, *_sql_param, [&isolate, &callback, &global](int i, std::vector<char*>&rows) {
		std::string* row_str = new std::string();
		json_array_stringify_s(rows, *row_str);
		v8::Handle<v8::Value> arg[2] = { 
			v8::Number::New(isolate, i), 
			sow_web_jsx::v8_str(isolate, row_str->c_str())
		};
		callback->Call(global, 2, arg);
		free(row_str);
		return;
	});
	pgsql->close();
	if (rec < 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, pgsql->get_last_error())));
		free(pgsql);
		return;
	};
	free(pgsql); global.Clear();
	callback.Clear();
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
};
void write_file_from_payload(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Contenent length required!!!")));
		return;
	}
	v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	size_t content_length = static_cast<size_t>(num->Value());
	if (content_length <= 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "No Contenent found!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[1]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
#if !defined(FAST_CGI_APP)
	FILE*fwstream;
	errno_t err = fopen_s(&fwstream, abs_path->data(), "w+");
#else
	FILE*fwstream = fopen(abs_path->data(), "w+");
	errno_t err = 0;
	if (fwstream == NULL || fwstream->stdio_stream == NULL) {
		err = -1;
	}
#endif//!FAST_CGI_APP
	if (err != 0) {
		free(abs_path);
		isolate->ThrowException(v8::Exception::TypeError(
			concat_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path->data())));
		return;
	}
	size_t read_length = 0;
	size_t len = 0;
	size_t write_len = 0;
	while (true) {
		char* buff;
		if (content_length > DATA_READ_CHUNK) {
			buff = (char*)malloc(DATA_READ_CHUNK + 1);
			len = DATA_READ_CHUNK;
		}
		else {
			buff = (char*)malloc(content_length + 1);
			len = content_length;
		}
		buff[len] = '\0';
#if defined(FAST_CGI_APP)
		std::cin.read(buff, len);
#else
		fread(buff, 1, len, stdin);
#endif//!FAST_CGI_APP
		write_len += fwrite(buff, 1, len, fwstream);
		if (ferror(fwstream)) {
			free(buff); free(abs_path);
			fclose(fwstream);
			isolate->ThrowException(v8::Exception::TypeError(
				sow_web_jsx::concat_msg(isolate, "Unable to write file!!! Server absolute path==>", abs_path->data())));
			return;
		}
		free(buff);
		content_length -= len;
		if (content_length <= 0)break;
	}
	fclose(stdin); fclose(fwstream);
	free(abs_path);
	args.GetReturnValue().Set(v8::Number::New(isolate, static_cast<double>(write_len)));
	return;
};
void read_payload(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Contenent length required!!!")));
		return;
	}
	if (!args[1]->IsFunction() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Callback required!!!")));
		return;
	}
	v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	size_t content_length = static_cast<size_t>(num->Value());
	v8::Persistent<v8::Function> cb;
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[1]));
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	v8::Local<v8::Object>global = args.Holder();
	size_t read_length = 0;
	size_t len = 0;
	//DATA_READ_CHUNK should be 8192 according to FCGX_Accept_r(FCGX_Request *reqDataPtr) line 2154 file fcgiapp.c

	while (true) {
		char* buff;
		if (content_length > DATA_READ_CHUNK) {
			buff = (char*)malloc(DATA_READ_CHUNK + 1);
			len = DATA_READ_CHUNK;
		}
		else {
			buff = (char*)malloc(content_length + 1);
			len = content_length;
		}
		buff[len] = '\0';
#if defined(FAST_CGI_APP)
		std::cin.read(buff, len);
#else
		fread(buff, 1, len, stdin);
#endif//!FAST_CGI_APP
		v8::Handle<v8::Value> arg[2] = {
			sow_web_jsx::v8_str(isolate, const_cast<const char*>(buff)),
			v8::Number::New(isolate, static_cast<double>(len))
		};
		free(buff);
		callback->Call(global, 2, arg);
		content_length -= len;
		if (content_length <= 0)break;
	}
	fclose(stdin);
	callback.Clear();  global.Clear();
	args.GetReturnValue().Set(v8::Number::New(isolate, static_cast<double>(content_length)));
};
void native_open_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (_is_cli == false) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Not Supported!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Process absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Argument(s) required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	const char* process_path = *utf_abs_path_str;
	v8::String::Utf8Value utf_arg_str(isolate, args[1]);
	int ret = sow_web_jsx::open_process(process_path, *utf_arg_str);
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Not found!!!")));
		return;
	};
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));
};
void get_prop_value(v8::Isolate* isolate, v8::Local<v8::Object> obj, const char* prop, std::string&out) {
	v8::Local<v8::Value> v8_str = obj->Get(sow_web_jsx::v8_str(isolate, prop));
	if (v8_str->IsNullOrUndefined())return;
	v8::String::Utf8Value utf8_str(isolate, v8_str);
	//v8_str.Clear();
	out = *utf8_str;
}
void get_req_process_info(v8::Isolate* isolate, v8::Local<v8::Object> pi, process_info &pri) {
	get_prop_value(isolate, pi, "start_in", pri.start_in);
	if (!pri.start_in.empty())
		pri.start_in = std::regex_replace(pri.start_in, std::regex("(?:/)"), "\\");
	get_prop_value(isolate, pi, "process_name", pri.process_name);
	get_prop_value(isolate, pi, "process_path", pri.process_path);
	if (!pri.process_path.empty())
		pri.process_path = std::regex_replace(pri.process_path, std::regex("(?:/)"), "\\");
	get_prop_value(isolate, pi, "title", pri.lp_title);
	get_prop_value(isolate, pi, "arg", pri.arg);
	if (_is_cli == false) {
		//Internal request defined
		if (pri.arg.empty()) 
			pri.arg = "I_REQ"; 
		else 
			pri.arg = "I_REQ " + pri.arg;
	}
	/*if (pri.arg.empty())
		pri.arg = "I_REQ";
	else
		pri.arg = "I_REQ " + pri.arg;*/
	pri.show_window = SW_HIDE;//SW_SHOWNORMAL;// CREATE_NO_WINDOW;//(int)get_prop_value(isolate, pi, "show_window");
	//wait_for_exit
	v8::Local<v8::Value> v8_bool = pi->Get(sow_web_jsx::v8_str(isolate, "wait_for_exit"));
	if (v8_bool->IsBoolean()) {
		pri.wait_for_exit = v8::Local<v8::Boolean>::Cast(v8_bool)->IsTrue() ? 1 : -1;
	}
	else {
		pri.wait_for_exit = -1;
	}
}
int spawn_uv_process(const process_info pi) {
	//http://docs.libuv.org/en/v1.x/guide/processes.html
	uv_loop_t *loop;
	uv_process_t child_req;
	uv_process_options_t options;
	loop = uv_default_loop();
	const char* args[3];
	args[0] = pi.process_path.c_str();//"C:\\web_jsx\\web_jsx_cgi.exe";
	args[1] = pi.arg.c_str();// "m.jsxh";
	args[2] = NULL;
	options.cwd = pi.start_in.c_str();////"C:\\web_jsx\\";
	options.exit_cb = NULL;
	options.file = pi.process_path.c_str();//"C:\\web_jsx\\web_jsx_cgi.exe";
	options.args = (char**)args;
	
	options.flags = UV_PROCESS_DETACHED;
	int r;
	if ((r = uv_spawn(loop, &child_req, &options))) {
		fprintf(stderr, "%s\n", uv_strerror(r));
		return 1;
	}
	fprintf(stderr, "Launched sleep with PID %d\n", child_req.pid);
	uv_unref((uv_handle_t*)&child_req);
	return uv_run(loop, UV_RUN_DEFAULT);
}
void throw_js_error(v8::Isolate* isolate, const char* err) {
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, err)));
	return;
}
//void kill_process_by_name(const char *process_name)
void native_kill_process_by_name(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "App name required!!!")));
		return;
	}
	v8::String::Utf8Value utf8_str(isolate, args[0]);
	const char*process_name = *utf8_str;
	//const wchar_t* p_name = L"notepad++.exe";
	//wchar_t* p_name = sow_web_jsx::ccr2ws(process_name);
	//const wchar_t* pr_name = sow_web_jsx::ccr2ws(process_name);
	int ret = sow_web_jsx::kill_process_by_name(process_name);
	//ret = wcscmp (p_name, pr_name) == 0 ? 1 : -1;
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));
}
void native_create_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Process info required!!!")));
		return;
	}
	process_info *pri = new process_info();
	v8::Local<v8::Object> pi = v8::Handle<v8::Object>::Cast(args[0]);
	get_req_process_info(isolate, pi, *pri);
	int ret = 0;
	if (_is_cli == false) {
		//No Child process available in web
		//SW_SHOWNORMAL
		pri->show_window = SW_SHOWNORMAL;
		pri->dw_creation_flags = CREATE_NO_WINDOW;
		pri->wait_for_exit = -1;
		ret = sow_web_jsx::create_process(pri);
	}
	else {
		v8::Local<v8::Value> js_reader = pi->Get(sow_web_jsx::v8_str(isolate, "reader"));
		if (!js_reader->IsFunction()) {
			std::string ptype;
			get_prop_value(isolate, pi, "process_type", ptype);
			if (ptype.empty())
				pri->dw_creation_flags = CREATE_NO_WINDOW;
			else {
				if (ptype == "CHILD")
					pri->dw_creation_flags = CREATE_NEW_PROCESS_GROUP;
				else
					pri->dw_creation_flags = CREATE_NO_WINDOW;
			}
			if (pri->dw_creation_flags == CREATE_NO_WINDOW)
				pri->wait_for_exit = -1;
			std::string().swap(ptype);
			ret = sow_web_jsx::create_process(pri);
		}
		else {
			pri->wait_for_exit = -1;
			v8::Persistent<v8::Function> cb;
			cb.Reset(isolate, v8::Local<v8::Function>::Cast(js_reader));
			v8::Local<v8::Object>global = args.Holder();
			v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
			ret = sow_web_jsx::read_child_process(pri, [&](long i, const char*buff) {
				v8::Handle<v8::Value> arg[2] = {
					v8::Number::New(isolate, i),
					sow_web_jsx::v8_str(isolate, buff)
				};
				callback->Call(global, 2, arg);
			});
		}
	}
	std::string().swap(pri->start_in);
	std::string().swap(pri->process_name);
	std::string().swap(pri->process_path);
	std::string().swap(pri->lp_title);
	std::string().swap(pri->arg);
	delete pri;
	pi.Clear();
	if (ret < 0) {
		switch (ret) {
		case -4 : throw_js_error(isolate, "process not found!!!"); return;
		case -10: throw_js_error(isolate, "process path required!!!"); return;
		case -9: throw_js_error(isolate, "Unable to execute shall command!!!"); return;
		default: throw_js_error(isolate, "Unknown Error defined!!!"); return;
		};
	}
	switch (ret) {
	case ERROR_FILE_NOT_FOUND: throw_js_error(isolate, "ERROR_FILE_NOT_FOUND!!!"); return;
	case ERROR_PATH_NOT_FOUND: throw_js_error(isolate, "ERROR_PATH_NOT_FOUND!!!"); return;
	case ERROR_DIRECTORY: throw_js_error(isolate, "ERROR_DIRECTORY!!!"); return;
	case ERROR_NOT_ENOUGH_MEMORY: throw_js_error(isolate, "ERROR_NOT_ENOUGH_MEMORY!!!"); return;
	case ERROR_INVALID_NAME: throw_js_error(isolate, "ERROR_INVALID_NAME!!!"); return;
	default:break;
	}
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));
	//throw_js_error(isolate, std::string("ret" + std::to_string(ret) + "arg:" + pri->arg + "; start_in:" + pri->start_in + "; process_name:" + pri->process_name + "; process_path:" + pri->process_path).c_str());
	
}
void native_create_child_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (_is_cli == false) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Not Supported!!!")));
		return;
	}
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Process absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Argument(s) required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	std::string process_path = *utf_abs_path_str;
	v8::String::Utf8Value utf_arg_str(isolate, args[1]);
	std::string arg = *utf_arg_str;
	process_path = std::regex_replace(process_path, std::regex("(?:/)"), "\\");
	long ret = sow_web_jsx::create_child_process(process_path.c_str(), arg.c_str());
	std::string().swap(arg); std::string().swap(process_path);
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Not found!!!")));
		return;
	};
	args.GetReturnValue().Set(v8::Number::New(isolate, ret));
};
void native_terminate_process(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate * isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "PID Required!!!")));
		return;
	}
	v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	uint64_t pid = num->ToInteger(isolate)->Value();
	int rec = 0;
#if defined(_WINDOWS_)
	rec = (int)sow_web_jsx::terminate_process((DWORD)pid);
#else
#error Not Implemented
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
};
void native_process_is_running(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate * isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "PID Required!!!")));
		return;
	}
	v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	uint64_t pid = num->ToInteger(isolate)->Value();
	int rec = 0;
#if defined(_WINDOWS_)
	rec = (int)sow_web_jsx::process_is_running((DWORD)pid);
#else
#error Not Implemented
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
};
void native_current_process_id(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate * isolate = args.GetIsolate();
	int rec = 0;
#if defined(_WINDOWS_)
	rec = (int)sow_web_jsx::current_process_id();
#else
#error !TODO
#endif//_WINDOWS_
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
};
void read_directory_regx(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Match pattern required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	auto directorys = new  std::vector<std::string>();
	int rec = 0;
	v8::String::Utf8Value utf_ext_str(isolate, args[1]);
	auto reg = new std::regex(*utf_ext_str);//"(html|aspx|jsx|php)"
	rec = sow_web_jsx::read_directory_sub_directory_x(abs_path->data(), *directorys, *reg);
	free(reg);
	free(abs_path);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (rec == EXIT_FAILURE) {
		free(directorys);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			sow_web_jsx::concat_msg(isolate, "Could not open directory==>", abs_path->data())
		);
		args.GetReturnValue().Set(v8_result);
		return;
	}
	std::vector<std::string>&json_obj = *directorys;
	v8::Local<v8::Array> directory_v8_array = v8::Array::New(isolate, (int)json_obj.size());
	for (size_t i = 0, l = json_obj.size(); i < l; ++i) {
		directory_v8_array->Set((int)i, sow_web_jsx::v8_str(isolate, json_obj[i].c_str()));
	}
	free(directorys);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "dir"),
		directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
};
void read_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//3:21 PM 12/24/2018
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	auto directorys = new  std::vector<std::string>();
	int rec = 0;
	if (args[1]->IsString() || !args[1]->IsNullOrUndefined()) {
		v8::String::Utf8Value utf_ext_str(isolate, args[1]);
		rec = sow_web_jsx::read_directory_sub_directory(abs_path->data(), *directorys, *utf_ext_str);
	}
	else {
		rec = sow_web_jsx::read_directory_sub_directory(abs_path->data(), *directorys, "A");
	}
	
	free(abs_path);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (rec == EXIT_FAILURE) {
		free(directorys);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			sow_web_jsx::concat_msg(isolate, "Could not open directory==>", abs_path->data())
		);
		args.GetReturnValue().Set(v8_result);
		return;
	}
	std::vector<std::string>&json_obj = *directorys;
	v8::Local<v8::Array> directory_v8_array = v8::Array::New(isolate, (int)json_obj.size());
	for (size_t i = 0, l= json_obj.size(); i < l; ++i) {
		directory_v8_array->Set((int)i, sow_web_jsx::v8_str(isolate, json_obj[i].c_str()));
	}
	free(directorys);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "staus_code"),
		v8::Number::New(isolate, 1)
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "dir"),
		directory_v8_array
	);
	directory_v8_array.Clear();
	args.GetReturnValue().Set(v8_result);
};
void native_write_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File data required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	v8::String::Utf8Value utf_data_str(isolate, args[1]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	v8::Handle<v8::Object> v8_result = sow_web_jsx::native_write_filei(isolate, *abs_path, *utf_data_str);
	args.GetReturnValue().Set(v8_result);
	v8_result.Clear(); delete abs_path;
};
void native_read_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	std::stringstream ssstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	size_t ret = sow_web_jsx::read_file(abs_path->data(), ssstream, true);
	delete abs_path;
	//const char* cstr2 = ssstream.str().c_str();
	//std::stringstream().swap(ssstream);
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (ret < 0) {
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, (double)ret)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			v8::String::NewFromUtf8(isolate, ssstream.str().c_str())
		);
		args.GetReturnValue().Set(v8_result);
		std::stringstream().swap(ssstream);
		return;
	}
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "staus_code"),
		v8::Number::New(isolate, (double)ret)
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "message"),
		v8::String::NewFromUtf8(isolate, "Success...")
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "data"),
		v8::String::NewFromUtf8(isolate, ssstream.str().c_str())
	);
	args.GetReturnValue().Set(v8_result);
	std::stringstream().swap(ssstream);
	return;
};
void native_write_from_file(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	size_t ret = sow_web_jsx::read_file(abs_path->data(), _body_stream,  true);
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "No file foud!!! Server absolute path==>", abs_path->data())));
		delete abs_path;
		return;
	}
	delete abs_path;
	return;
};
void native_delete_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Directory required!!!")));
		return;
	}
	v8::String::Utf8Value utf8_path_str(isolate, args[0]);
	auto abs_dir = new std::string();
	abs_dir->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf8_path_str, *abs_dir);
	int rec = sow_web_jsx::delete_dir(abs_dir->data());
	if (rec > 0)
		args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "Success"));
	else
		isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, "Directory does not exists!!! Server absolute path==>", abs_dir->data())));
	delete abs_dir;
	return;

};
void native_create_directory(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Server path required!!!")));
		return;
	}
	v8::String::Utf8Value utf8_path_str(isolate, args[0]);
	auto abs_dir = new std::string();
	abs_dir->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf8_path_str, *abs_dir);
	int rec = sow_web_jsx::create_directory(abs_dir->data());
	if (rec > 0) {
		args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "Success"));
	}
	else {
		if (rec == -1) {
			args.GetReturnValue().Set(sow_web_jsx::concat_msg(isolate, "Directory already exists!!! Dir#", abs_dir->data()));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::concat_msg(isolate, "Unknown error please retry!!! Dir#", abs_dir->data()));
		}
	}
	delete abs_dir;
	return;
	
};
void async_func(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate *isolate = args.GetIsolate();
	if (!args[0]->IsFunction()) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Function required!!!")));
		return;
	}
	v8::Persistent<v8::Function> cb;
	cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, cb);
	isolate->Enter();
	std::future<int> result = std::async(std::launch::async | std::launch::deferred, [=]() {
		try {
			v8::HandleScope			handleScope(isolate);
			v8::Isolate::Scope		isolate_scope(isolate);
			v8::Local<v8::Context>	context = isolate->GetCurrentContext();
			v8::Context::Scope		context_scope(context);
			v8::Local<v8::Object>	global = context->Global();
			v8::Handle<v8::Value> arg[1] = {
				v8::Number::New(isolate, 0)
			};
			callback->Call(global, 1, arg);
			context.Clear(); global.Clear();
			isolate->Exit();
			try {
				std::stringstream ss;
				ss << std::this_thread::get_id();
				uint64_t id = std::stoull(ss.str());
				return (int)id;
			} catch (...) {
				return 1;
			}
		} catch (...) {
			return -1;
		}
	});
	int rec = result.get();
	cb.Reset(); 
	callback.Clear();
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
};
void set_time_out_func(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate * isolate = args.GetIsolate();
	if (!args[0]->IsFunction() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Callback Required!!!")));
		return;
	}
	uint64_t milliseconds = 0;
	if (args[1]->IsNumber() ) {
		v8::Local<v8::Number> num = args[1]->ToNumber(isolate);
		milliseconds = num->ToInteger(isolate)->Value();
	}
	async_func_arg*afa = new async_func_arg();
	afa->cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	int rec = sow_web_jsx::set_time_out(isolate, [](async_func_arg* arg) {
		try {
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope			handleScope(isolate);
			v8::Isolate::Scope		isolate_scope(isolate);
			v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, v8::MaybeLocal<v8::ObjectTemplate>());
			v8::Context::Scope		context_scope(context);
			v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, arg->cb);
			v8::Handle<v8::Value> carg[1] = {
				v8::String::NewFromUtf8(isolate, "START")
			};
			callback->Call(context->Global(), 1, carg);
			context.Clear();
			isolate->Exit();
		} catch (...) {}
	}, afa, milliseconds);
	args.GetReturnValue().Set(v8::Number::New(isolate, rec));
};
void generate_pdf(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//11:14 PM 12/4/2018
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(isolate, "Object required!!!")));
			return;
		}
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Value> v8_path_str = config->Get(sow_web_jsx::v8_str(isolate, "path"));
		if (v8_path_str->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::Error(
				v8::String::NewFromUtf8(isolate, "Output path required!!!")));
			return;
		}
		v8::Local<v8::Value> v8_url_str = config->Get(sow_web_jsx::v8_str(isolate, "url"));
		bool form_body = false;
		v8::Local<v8::Boolean> b = v8::Local<v8::Boolean>::Cast(config->Get(sow_web_jsx::v8_str(isolate, "from_body")));
		if (b->IsTrue()) {
			form_body = true;
		}
		else {
			if (!args[1]->IsString() || args[0]->IsNullOrUndefined()) {
				if (!v8_url_str->IsString() && v8_url_str->IsNullOrUndefined()) {
					config.Clear();
					isolate->ThrowException(v8::Exception::TypeError(
						v8::String::NewFromUtf8(isolate, "Body string required!!!")));
					return;
				}
			}
		}
		v8::String::Utf8Value utf8_path_str(isolate, v8_path_str);
		auto abs_path = new std::string();
		abs_path->append(_root_dir->data());
		sow_web_jsx::get_server_map_path(*utf8_path_str, *abs_path);
		auto dir = new std::string();
		sow_web_jsx::get_dir_from_path(*abs_path, *dir);
		if (!sow_web_jsx::dir_exists(dir->data())) {
			isolate->ThrowException(v8::Exception::Error(sow_web_jsx::concat_msg(isolate, dir->data(), " does not exists!!! Please at first create directory.")));
			delete dir; delete abs_path; config.Clear();
			return;
		}
		delete dir;
		v8::Local<v8::Value> v8_global_settings_str = config->Get(sow_web_jsx::v8_str(isolate, "global_settings"));
		auto wgs_settings = new std::map<const char*, const char*>();
		if (!v8_global_settings_str->IsNullOrUndefined() && v8_global_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_global_settings_object = v8::Local<v8::Object>::Cast(v8_global_settings_str);
			n_help::v8_object_loop(isolate, v8_global_settings_object, *wgs_settings);
			v8_global_settings_str.Clear();
			v8_global_settings_object.Clear();
		}
		v8::Local<v8::Value> v8_object_settings_str = config->Get(sow_web_jsx::v8_str(isolate, "object_settings"));
		auto wos_settings = new std::map<const char*, const char*>();
		if (!v8_object_settings_str->IsNullOrUndefined() && v8_object_settings_str->IsObject()) {
			v8::Local<v8::Object>v8_object_settings_object = v8::Local<v8::Object>::Cast(v8_object_settings_str);
			n_help::v8_object_loop(isolate, v8_object_settings_object, *wos_settings);
			v8_object_settings_str.Clear();
			v8_object_settings_object.Clear();
		}
		config.Clear();
		pdf_ext::pdf_generator* pdf_gen = new pdf_ext::pdf_generator();
		malloc(sizeof pdf_gen);
		int rec = 0;
		rec = pdf_gen->init(true, *wgs_settings, *wos_settings);
		delete wgs_settings; delete wos_settings;
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		if (rec < 0) {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, pdf_gen->get_status_msg())
			);
			args.GetReturnValue().Set(v8_result);
			v8_result.Clear();
			delete pdf_gen;
			return;
		}

		if (!form_body) {
			if (v8_url_str->IsString() && !v8_url_str->IsNullOrUndefined()) {
				v8::String::Utf8Value utf8_url_str(isolate, v8_url_str);
				rec = pdf_gen->generate_from_url(*utf8_url_str, abs_path->c_str());
			}
			else {
				v8::String::Utf8Value utf8_body_str(isolate, args[1]);
				rec = pdf_gen->generate_to_path(*utf8_body_str, abs_path->c_str());
			}

		}
		else {
			auto str = new std::string(_body_stream.str());
			malloc(sizeof str);
			rec = pdf_gen->generate_to_path(str->data(), abs_path->c_str());
			delete str;
		}
		delete abs_path;
		if (rec > 0) {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, "Success")
			);
		}
		else {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, pdf_gen->get_status_msg())
			);
		}
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
		delete pdf_gen;
		return;
	} catch (std::exception&e) {
		args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, e.what()));
	}
};
void generate_pdf_from_body(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (n_help::write_http_status(*_http_status, true) < 0) {
			args.GetReturnValue().Set(v8::Number::New(isolate, -1));
			return;
		};
		std::string*str = new std::string(_body_stream.str());
		std::stringstream().swap(_body_stream);
		malloc(sizeof str);
		std::string str_output;
		pdf_ext::pdf_generator* pdf_gen = new pdf_ext::pdf_generator();
		malloc(sizeof pdf_gen);
		
		pdf_gen->init(true);
		int rec = pdf_gen->generate(str->c_str(), str_output);
		delete str;
		if (rec < 0) {
			pdf_gen->dispose();
			_body_stream << pdf_gen->get_status_msg();
			args.GetReturnValue().Set(v8::Number::New(isolate, rec));
			return;
		}
		//body_stream << "\r\n";
		_body_stream << str_output;
		std::string().swap(str_output);
		//body_stream << "\r\n\r\n";
		n_help::add_header(*_headers, "wkhtmltopdf_version", pdf_gen->version);
		pdf_gen->dispose();
		delete pdf_gen;
		n_help::add_header(*_headers, "Accept-Ranges", "bytes");
		n_help::add_header(*_headers, "Content-Type", "application/pdf");
		//n_help::add_header(*_headers, "Content-Disposition", "attachment;filename=\"auto.pdf\"");
		//n_help::add_header(*_headers, "Content-Type", "application/octet-stream");
		args.GetReturnValue().Set(v8::Number::New(isolate, rec));
		//delete data; 
	} catch (std::runtime_error&e) {
		std::stringstream().swap(_body_stream);
		_body_stream << "PDF Generation failed!!!\r\n";
		_body_stream << "ERROR==>" << e.what();
		args.GetReturnValue().Set(v8::Number::New(isolate, -1));
	}
	return;
};
void set_cookie(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args[0]->IsNullOrUndefined() || !args[0]->IsString()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "HTTP Cookie required!!!")));
		return;
	}
	v8::String::Utf8Value resp_cookie_str(isolate, args[0]);
	const char* cook_val = *resp_cookie_str;
	auto itr = std::find(_cookies->begin(), _cookies->end(), cook_val);
	if (itr != _cookies->end()) {
		_cookies->erase(itr);
	}
	_cookies->push_back(cook_val);
	args.GetReturnValue().Set(v8::Number::New(isolate, 1));
};
void http_status(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Server Protocol Required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "HTTP Status Code Required!!!")));
		return;
	}
	v8::String::Utf8Value server_proto_str(isolate, args[0]);
	v8::String::Utf8Value status_code_str(isolate, args[1]);
	auto desc = new std::string(*server_proto_str);
	desc->append(" ");
	desc->append(*status_code_str);
	desc->append(" ");
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		desc->append("Unspecified Description");
	}
	else {
		v8::String::Utf8Value dec_str(isolate, args[2]);
		desc->append(*dec_str);
	}
	n_help::add_http_status(*_http_status, *desc);
	delete desc;
	return;
};
void response_write_header(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Header Key string required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Header Description string required!!!")));
		return;
	}
	v8::String::Utf8Value key_str(isolate, args[0]);
	v8::String::Utf8Value description_str(isolate, args[1]);
	const char* ckey = *key_str;
	/*if (strcmp(ckey, "X-Powered-By") == 0 || strcmp(ckey, "X-Process-By") == 0 || strcmp(ckey, "Content-Encoding") == 0) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, sow_web_jsx::concat_msg("Reserve header key not allowed ==>", ckey))));
		return;
	}*/
	const char* desc = *description_str;
	n_help::add_header(*_headers, ckey, desc);
	return;
};
void response_write(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (args[0]->IsNullOrUndefined())return;
	v8::String::Utf8Value utf8_str(isolate, args[0]);
	_body_stream << *utf8_str;
	return;
};
void http_request(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	try {
		if (!args[0]->IsObject() || args[0]->IsNullOrUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Object required!!!")));
			return;
		}
		v8::Local<v8::Object> config = v8::Handle<v8::Object>::Cast(args[0]);
		v8::Local<v8::Value> v8_url_str = config->Get(sow_web_jsx::v8_str(isolate, "url"));
		if (v8_url_str->IsNullOrUndefined() || !v8_url_str->IsString()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Url should be string!!!")));
			return;
		}
		v8::Local<v8::Value> v8_method_str = config->Get(sow_web_jsx::v8_str(isolate, "method"));
		v8::String::Utf8Value utf_method_str(isolate, v8_method_str);
		std::string method(*utf_method_str);
		if (v8_method_str->IsNullOrUndefined() || !v8_method_str->IsString()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Method should be string!!!")));
			return;
		}
		v8::Local<v8::Value>v8_payload_val = config->Get(sow_web_jsx::v8_str(isolate, "body"));
		if (method == "POST") {
			if (v8_payload_val->IsNullOrUndefined() || !v8_payload_val->IsString()) {
				isolate->ThrowException(v8::Exception::TypeError(
					v8::String::NewFromUtf8(isolate, "Payload required for POST Request!!!")));
				return;
			}
		}
		v8::String::Utf8Value utf_url_str(isolate, v8_url_str);
		http_client::http_request* http = new http_client::http_request(*utf_url_str, true);
		v8_url_str.Clear();
		v8::Local<v8::Value>v8_header_str = config->Get(sow_web_jsx::v8_str(isolate, "header"));
		if (!v8_header_str->IsNullOrUndefined() && v8_header_str->IsArray()) {
			v8::Local<v8::Array> harr = v8::Local<v8::Array>::Cast(v8_header_str);
			for (uint32_t i = 0; i < harr->Length(); i++) {
				v8::Local<v8::Value>v8_val = harr->Get(i);
				if (v8_val->IsNullOrUndefined() || !v8_val->IsString())continue;
				v8::String::Utf8Value key (isolate, v8_val);
				http->set_header(*key);
			}
			v8_header_str.Clear();
		}
		v8::Local<v8::Value>v8_cookie_str = config->Get(sow_web_jsx::v8_str(isolate, "cookie"));
		if (!v8_cookie_str->IsNullOrUndefined() && v8_cookie_str->IsString()) {
			v8::String::Utf8Value utf_cook_str (isolate, v8_cookie_str);
			http->set_cookie(*utf_cook_str);
		}
		else if (!v8_cookie_str->IsNullOrUndefined() && v8_cookie_str->IsArray()) {
			v8::Local<v8::Array> carr = v8::Local<v8::Array>::Cast(v8_cookie_str);
			std::string* cookies = new std::string();
			for (uint32_t i = 0; i < carr->Length(); i++) {
				v8::Local<v8::Value>v8_val = carr->Get(i);
				if (v8_val->IsNullOrUndefined() || !v8_val->IsString())continue;
				v8::String::Utf8Value key (isolate, v8_val);
				if (i == 0) {
					cookies->append(*key); continue;
				}
				cookies->append("; ");
				cookies->append(*key);
			}
			http->set_cookie(cookies->c_str());
			delete cookies;
			v8_cookie_str.Clear();
		}
		std::string* resp_header = new std::string();
		std::string* resp_body = new std::string();
		int rec = 0;
		if (method == "POST") {
			v8::String::Utf8Value v8_payload_str (isolate, v8_payload_val);
			//rec = http->post(*v8_payload_str, *resp_header, *resp_body);
			const char* payload_str = *v8_payload_str;
			std::future<int> result = std::async(std::launch::async | std::launch::deferred, [=]() {
				return http->post(payload_str, *resp_header, *resp_body);
			});
			rec = result.get();
			v8_payload_val.Clear();
		}
		else {
			std::future<int> result = std::async(std::launch::async | std::launch::deferred, [=]() {
				return http->get(*resp_header, *resp_body);;
			});
			rec = result.get();
			//rec = http->get(*resp_header, *resp_body);
		}
		config.Clear();
		v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
		if (rec < 0) {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, http->get_last_error())
			);
		}
		else {
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_val"),
				v8::Number::New(isolate, rec)
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "ret_msg"),
				v8::String::NewFromUtf8(isolate, "success")
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "response_header"),
				v8::String::NewFromUtf8(isolate, resp_header->c_str())
			);
			v8_result->Set(
				v8::String::NewFromUtf8(isolate, "response_body"),
				v8::String::NewFromUtf8(isolate, resp_body->c_str())
			);
		}
		delete http; delete resp_header; delete resp_body;
		args.GetReturnValue().Set(v8_result);
		v8_result.Clear();
		return;
	} catch (std::runtime_error&e) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, e.what()));
		return;
	} catch (...) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, "Unknown!!!"));
	}
	
};
//#include <iosfwd>
//5:08 PM 11/28/2018
/*
HTTP/1.1 301 Moved Permanently
Location: http://www.example.org/index.asp
*/
void sleep_func(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsNumber() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Sleep time Required!!!")));
		return;
	}
	v8::Local<v8::Number> num = args[0]->ToNumber(isolate);
	int64_t milliseconds = num->ToInteger(isolate)->Value();
#if defined(_WINDOWS_)
	Sleep((DWORD)milliseconds);
#else
#error Not Implemented
#endif//!_WINDOWS_
};
void response_redirect(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Server Protocol Required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Redirect location required!!!")));
		return;
	}
	v8::String::Utf8Value server_proto_str(isolate, args[0]);
	v8::String::Utf8Value url_str(isolate, args[1]);
	auto desc = new std::string(*server_proto_str);
	//desc->append(" 301 Moved Permanently\n");
	desc->append(" 303 See Other\n");
	desc->append("Location: ");
	desc->append(*url_str);
	n_help::add_http_status(*_http_status, *desc);
	delete desc;
};
void encrypt_source(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
#error Not Implemented
#else
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not Implemented!!!")));
	return;
#endif//!__client_build
};
void decrypt_source(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
#error Not Implemented
#else
	isolate->ThrowException(v8::Exception::Error(
		v8::String::NewFromUtf8(isolate, "Not Implemented!!!")));
	return;
#endif//!__client_build
};
void async_t(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate * isolate = args.GetIsolate();
	async_func_arg*afa = new async_func_arg();
	afa->cb.Reset(isolate, v8::Local<v8::Function>::Cast(args[0]));
	sow_web_jsx::acync_init(isolate, [](async_func_arg* arg) {
		try {
			v8::Isolate* isolate = v8::Isolate::GetCurrent();
			v8::HandleScope			handleScope(isolate);
			v8::Isolate::Scope		isolate_scope(isolate);
			v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, v8::MaybeLocal<v8::ObjectTemplate>());
			v8::Context::Scope		context_scope(context);
			v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, arg->cb);
			v8::Handle<v8::Value> carg[1] = {
				v8::String::NewFromUtf8(isolate, "START")
			};
			callback->Call(context->Global(), 1, carg);
			context.Clear();
			isolate->Exit();
		} catch (...) {}
	}, afa);
	args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "START"));
};
/*[zgip]*/
void gzip_compress_write() {
	if (n_help::write_http_status(*_http_status, true) < 0) {
		std::stringstream().swap(_body_stream);
		n_help::write_http_status(*_http_status, false);
		n_help::write_cookies(*_cookies);
		delete _http_status; delete _headers; delete _cookies;
		std::cout << "\r\n";
		fflush(stdout);
		return;
	};
	delete _http_status;
	n_help::write_header(*_headers);
	delete _headers;
	n_help::write_cookies(*_cookies);
	delete _cookies;
	std::ios::sync_with_stdio(false);
	std::cout << "\r\n";
	std::future<void> result = std::async(std::launch::async | std::launch::deferred, [=]() {
		gzip::compress_gzip(_body_stream, std::cout);
		std::stringstream().swap(_body_stream);
	});
	result.get();
	fflush(stdout);
};
/*[/zgip]*/
void sow_web_jsx::wrapper::clear_cache() {
	if (!_is_cli)return;
	if (_is_flush == true)return;
	_is_flush = true;
	delete _root_dir;
};
//9:32 PM 11/22/2018
void sow_web_jsx::wrapper::response_body_flush() {
	if (_is_flush == true)return;
	_is_flush = true;
	if (n_help::is_gzip_encoding(*_headers) == 1 ) {
		gzip_compress_write();
		return;
	}
	if (n_help::write_http_status(*_http_status, true ) < 0) {
		std::stringstream().swap(_body_stream);
		n_help::write_http_status(*_http_status, false);
		n_help::write_cookies(*_cookies);
		delete _http_status; delete _headers; delete _cookies;
		std::cout << "\r\n";
		fflush(stdout);
		//std::cout << std::flush;
		return;
	}
	std::ios::sync_with_stdio(false);
	delete _http_status;
	n_help::write_header(*_headers);
	delete _headers;
	n_help::write_cookies(*_cookies);
	delete _cookies;
	fflush(stdout);
	std::cout << "\r\n";
	//SET_BINARY_MODE(stdout);
	SET_BINARY_MODE_OUT();
	std::copy(std::istreambuf_iterator<char>(_body_stream),
		std::istreambuf_iterator<char>(),
		std::ostream_iterator<char>(std::cout)
	);
	//std::cout << std::flush;
	fflush(stdout);
	std::stringstream().swap(_body_stream);
	return;
};
void response_clear(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	std::stringstream().swap(_body_stream);
};
void get_response_body(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	std::string* str = new std::string(_body_stream.str());
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, str->c_str()));
	delete str;
};
void encrypt_str(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Plain text required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "KEY required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "IV required!!!")));
		return;
	}
	v8::String::Utf8Value utf_plain_text(isolate, args[0]);
	v8::String::Utf8Value utf_key(isolate, args[1]);
	v8::String::Utf8Value utf_iv(isolate, args[2]);
	try {
		//void encrypt(const char*plain_text, const char*key, const char *iv, std::string&encrypt_text);
		std::string* encrypted_text = new std::string();
		std::string key(*utf_key);
		std::string iv(*utf_iv);
		std::string plain_text(*utf_plain_text);
		int rec = crypto::encrypt(plain_text.c_str(), key.c_str(), iv.c_str(), *encrypted_text);
		std::string().swap(key); std::string().swap(iv); std::string().swap(plain_text);
		if (rec < 0) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::concat_msg(isolate, "Unable to encrypt plain text.==>", encrypted_text->data())));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, encrypted_text->c_str()));
		}
		delete encrypted_text;
	} catch (std::runtime_error&e) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, e.what())));
	}
};
void decrypt_str(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Plain text required!!!")));
		return;
	}
	if (!args[1]->IsString() || args[1]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "KEY required!!!")));
		return;
	}
	if (!args[2]->IsString() || args[2]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "IV required!!!")));
		return;
	}
	v8::String::Utf8Value utf_encrypted_text(isolate, args[0]);
	v8::String::Utf8Value utf_key(isolate, args[1]);
	v8::String::Utf8Value utf_iv(isolate, args[2]);
	try {
		std::string key(*utf_key);
		std::string iv(*utf_iv);
		std::string encrypted_text(*utf_encrypted_text);
		//void encrypt(const char*plain_text, const char*key, const char *iv, std::string&encrypt_text);
		std::string* decrypted_text = new std::string();
		int rec = crypto::decrypt(encrypted_text.c_str(), key.c_str(), iv.c_str(), *decrypted_text);
		std::string().swap(key); std::string().swap(iv); std::string().swap(encrypted_text);
		if (rec < 0) {
			isolate->ThrowException(v8::Exception::Error(
				sow_web_jsx::concat_msg(isolate, "Unable to decrypt encrypted text.==>", decrypted_text->data())));
		}
		else {
			args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, decrypted_text->c_str()));
		}
		delete decrypted_text;
	} catch (std::runtime_error&e) {
		args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, e.what()));
	}
};
void hex_to_string(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Hex string required!!!")));
		return;
	}
	v8::String::Utf8Value utf_hex_str(isolate, args[0]);
	std::string* hex_str = new std::string(*utf_hex_str);
	std::string* plain_str = new std::string("");
	crypto::hex_to_string(*hex_str, *plain_str);
	free(hex_str);
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, plain_str->data()));
	free(plain_str);
	return;
};
void string_to_hex(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Plain string required!!!")));
		return;
	}
	v8::String::Utf8Value utf_plain_str(isolate, args[0]);
	std::string *plain_str = new std::string(*utf_plain_str);
	std::string* hex_str = new std::string("");
	crypto::string_to_hex(*plain_str, *hex_str);
	free(plain_str);
	args.GetReturnValue().Set(sow_web_jsx::v8_str(isolate, hex_str->data()));
	free(hex_str);
	return;
};
//9:32 PM 11/22/2018
void require(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();
	if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "File absolute path required!!!")));
		return;
	}
#if defined(__client_build)
#error Not Implemented
#endif//!__client_build
	v8::String::Utf8Value utf_abs_path_str(isolate, args[0]);
	auto abs_path = new std::string();
	abs_path->append(_root_dir->data());
	sow_web_jsx::get_server_map_path(*utf_abs_path_str, *abs_path);
	std::string source_str("");
#if defined(__client_build)
#error Not Implemented
#else
	size_t ret = sow_web_jsx::read_file(abs_path->data(), source_str, true);
	delete abs_path;
	if (ret < 0) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, source_str.c_str())));
		return;
	}
#endif//!__client_build
	v8::HandleScope handle_scope(isolate);
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	v8::Local<v8::ObjectTemplate> module_object = v8::ObjectTemplate::New(isolate);
	module_object->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	v8_global->Set(isolate, "module", module_object);
	v8_global->Set(isolate, "exports", v8::ObjectTemplate::New(isolate));
	/*[IO/file read/write]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write]*/
	/*[Sys Object]*/
	v8::Local<v8::ObjectTemplate> sys_object = v8::ObjectTemplate::New(isolate);
	sys_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	sys_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	sys_object->Set(isolate, "create_directory", v8::FunctionTemplate::New(isolate, native_create_directory));

	sys_object->Set(isolate, "delete_directory", v8::FunctionTemplate::New(isolate, native_delete_directory));
	sys_object->Set(isolate, "create_process", v8::FunctionTemplate::New(isolate, native_create_process));
	sys_object->Set(isolate, "create_child_process", v8::FunctionTemplate::New(isolate, native_create_child_process));
	sys_object->Set(isolate, "open_process", v8::FunctionTemplate::New(isolate, native_open_process));
	sys_object->Set(isolate, "terminate_process", v8::FunctionTemplate::New(isolate, native_terminate_process));
	sys_object->Set(isolate, "current_process_id", v8::FunctionTemplate::New(isolate, native_current_process_id));
	sys_object->Set(isolate, "process_is_running", v8::FunctionTemplate::New(isolate, native_process_is_running));
	sys_object->Set(isolate, "kill_process_by_name", v8::FunctionTemplate::New(isolate, native_kill_process_by_name));
	v8_global->Set(isolate, "sys", sys_object);
	/*[/Sys Object]*/
	/*[crypto]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	//
	v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto]*/
	/*[http_request]*/
	v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string));
	v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex));
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, async_func));
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, sleep_func));
	v8_global->Set(isolate, "__async_t", v8::FunctionTemplate::New(isolate, async_t));
	v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, v8_global);
	v8::Context::Scope context_scope(context);
	// Compile the source code.
	v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, sow_web_jsx::v8_str(isolate, source_str.c_str()));
	std::string().swap(source_str);
	if (script.IsEmpty()) {
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Unable to compile script. Check your script than try again.")));
		context.Clear();
		v8_global.Clear();
		return;
	}
	// Run the script to get the result.
	script.ToLocalChecked()->Run(context);
	v8::Local<v8::Object> jsGlobal =
		context->Global()->GetPrototype().As<v8::Object>();
	v8::Local<v8::Object> modules = v8::Handle<v8::Object>::Cast(jsGlobal->Get(sow_web_jsx::v8_str(isolate, "module")));
	args.GetReturnValue().Set(modules->Get(sow_web_jsx::v8_str(isolate, "exports")));
	modules.Clear();
	jsGlobal.Clear(); context.Clear();
	v8_global.Clear();
	return;
};
v8::Local<v8::Context> sow_web_jsx::wrapper::get_context(v8::Isolate* isolate, std::map<std::string, std::map<std::string, std::string>>& ctx) {
	//internal_global_ctx* js_ctx = new internal_global_ctx();
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = v8::ObjectTemplate::New(isolate);
	for (auto itr = ctx.begin(); itr != ctx.end(); ++itr) {
		auto key = itr->first;
		auto obj = itr->second;
		if (key == "global") {
			for (auto gitr = obj.begin(); gitr != obj.end(); ++gitr) {
				ctx_object->Set(isolate, (gitr->first).c_str(), v8_str(isolate, gitr->second.c_str()));
			}
			//ig_ctx.root_dir = new std::string(obj["root_dir"]);
			_root_dir = new std::string(obj["root_dir"]);
			continue;
		}
		v8::Local<v8::ObjectTemplate> object = v8::ObjectTemplate::New(isolate);
		for (auto oitr = obj.begin(); oitr != obj.end(); ++oitr) {
			object->Set(isolate, (oitr->first).c_str(), v8_str(isolate, oitr->second.c_str()));
		}
		if (key == "request") {
			object->Set(isolate, "_read_payload", v8::FunctionTemplate::New(isolate, read_payload));
		}
		ctx_object->Set(isolate, key.c_str(), object);
	}
	//sow_web_jsx::wrapper::response_body_flush
	/*[context.respons....]*/
	v8::Local<v8::ObjectTemplate> response_object = v8::ObjectTemplate::New(isolate);
	response_object->Set(isolate, "_write", v8::FunctionTemplate::New(isolate, response_write));
	response_object->Set(isolate, "header", v8::FunctionTemplate::New(isolate, response_write_header));
	response_object->Set(isolate, "write_from_file", v8::FunctionTemplate::New(isolate, native_write_from_file));
	v8::Local<v8::ObjectTemplate> body_object = v8::ObjectTemplate::New(isolate);
	//body_object->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, response_body_flush));
	body_object->Set(isolate, "flush", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		isolate->ThrowException(v8::Exception::Error(
			v8::String::NewFromUtf8(isolate, "Should not here.")));
		return;
	}));
	body_object->Set(isolate, "write_as_pdf", v8::FunctionTemplate::New(isolate, generate_pdf_from_body));
	response_object->Set(isolate, "body", body_object);
	response_object->Set(isolate, "clear", v8::FunctionTemplate::New(isolate, response_clear));
	response_object->Set(isolate, "status", v8::FunctionTemplate::New(isolate, http_status));
	response_object->Set(isolate, "cookie", v8::FunctionTemplate::New(isolate, set_cookie));
	response_object->Set(isolate, "_redirect", v8::FunctionTemplate::New(isolate, response_redirect));
	response_object->Set(isolate, "_as_gzip", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		n_help::add_header(*_headers, "Content-Encoding", "gzip");
		return;
	}));
	/*auto js_global = new internal_global_ctx();
	js_global->body_stream << "";*/
	response_object->Set(isolate, "_as_gzip", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* isolate = args.GetIsolate();
		n_help::add_header(*_headers, "Content-Encoding", "gzip");
		return;
	}));
	ctx_object->Set(isolate, "response", response_object);
	/*[/context.respons....]*/
	v8_global->Set(isolate, "context", ctx_object);
	/*[IO/file read/write....]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	io_object->Set(isolate, "_write_file_from_payload", v8::FunctionTemplate::New(isolate, write_file_from_payload));
	v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write....]*/
	/*[Sys Object]*/
	v8::Local<v8::ObjectTemplate> sys_object = v8::ObjectTemplate::New(isolate);
	sys_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	sys_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	sys_object->Set(isolate, "create_directory", v8::FunctionTemplate::New(isolate, native_create_directory));
	sys_object->Set(isolate, "delete_directory", v8::FunctionTemplate::New(isolate, native_delete_directory));
	sys_object->Set(isolate, "create_process", v8::FunctionTemplate::New(isolate, native_create_process));
	sys_object->Set(isolate, "terminate_process", v8::FunctionTemplate::New(isolate, native_terminate_process));
	sys_object->Set(isolate, "current_process_id", v8::FunctionTemplate::New(isolate, native_current_process_id));
	sys_object->Set(isolate, "process_is_running", v8::FunctionTemplate::New(isolate, native_process_is_running));
	sys_object->Set(isolate, "create_child_process", v8::FunctionTemplate::New(isolate, native_create_child_process));
	sys_object->Set(isolate, "open_process", v8::FunctionTemplate::New(isolate, native_open_process));
	sys_object->Set(isolate, "kill_process_by_name", v8::FunctionTemplate::New(isolate, native_kill_process_by_name));
	v8_global->Set(isolate, "sys", sys_object);
	/*[/Sys Object]*/
	/*[NpgSql....]*/
	v8::Local<v8::ObjectTemplate> npgsql_object = v8::ObjectTemplate::New(isolate);
	npgsql_object->Set(isolate, "execute_io", v8::FunctionTemplate::New(isolate, npgsql_execute_io));
	npgsql_object->Set(isolate, "execute_scalar", v8::FunctionTemplate::New(isolate, npgsql_execute_scalar));
	npgsql_object->Set(isolate, "execute_non_query", v8::FunctionTemplate::New(isolate, npgsql_execute_non_query));
	npgsql_object->Set(isolate, "data_reader", v8::FunctionTemplate::New(isolate, npgsql_data_reader));
	v8_global->Set(isolate, "npgsql", npgsql_object);
	/*[/NpgSql....]*/
	/*[pdf_generator....]*/
	v8::Local<v8::ObjectTemplate> pdf_object = v8::ObjectTemplate::New(isolate);
	pdf_object->Set(isolate, "generate_pdf", v8::FunctionTemplate::New(isolate, generate_pdf));
	v8_global->Set(isolate, "native_pdf", pdf_object);
	/*[/pdf_generator....]*/
	/*[crypto....]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto....]*/
	/*[http_request]*/
	v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	/*[require]*/
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[/require]*/
	v8_global->Set(isolate, "__get_response_body", v8::FunctionTemplate::New(isolate, get_response_body));
	//
	v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string));
	v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex));
	v8_global->Set(isolate, "__async_t", v8::FunctionTemplate::New(isolate, async_t));
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, async_func));
	v8_global->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, set_time_out_func));
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, sleep_func));
	//
	_headers = new std::map<std::string, std::string>();
	malloc(sizeof _headers);
	_cookies = new std::vector<std::string>();
	malloc(sizeof _cookies);
	_http_status = new std::vector<std::string>();
	malloc(sizeof _http_status);
	/*[Create C++ Internal Context]*/
	//v8::Local<v8::Context> v8_ctx = v8::Context::New(isolate, nullptr, v8_global/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
	//js_ctx.is_flush = 0;
	//js_ctx.headers = new std::map<std::string, std::string>();
	//js_ctx.cookies = new std::vector<std::string>();
	//js_ctx.http_status = new std::vector<std::string>();
	//js_ctx.body_stream = std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	/*[/Create C++ Internal Context]*/
	//sow_web_jsx::wrap_ctx(isolate, v8_ctx, &js_ctx);
	//return v8_ctx;
	//v8_global->SetInternalFieldCount(1);
	return v8::Context::New(isolate, nullptr, v8_global/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
}
v8::Local<v8::Context> sow_web_jsx::wrapper::get_console_context(v8::Isolate * isolate, std::map<std::string, std::string>&ctx) {
	_is_cli = true;
	v8::Local<v8::ObjectTemplate> v8_global = v8::ObjectTemplate::New(isolate);
	v8::Local<v8::ObjectTemplate> ctx_object = v8::ObjectTemplate::New(isolate);
	for (auto itr = ctx.begin(); itr != ctx.end(); ++itr) {
		if (itr->first == "root_dir") {
			_root_dir = new std::string(itr->second);
		}
		ctx_object->Set(isolate, itr->first.c_str(), v8_str(isolate, itr->second.c_str()));
	}
	v8_global->Set(isolate, "env", ctx_object);
	/*v8_global->Set(isolate, "__root_dir", v8_str(isolate, root_dir));
	v8_global->Set(isolate, "__path_translated", v8_str(isolate, path_translated));
	v8_global->Set(isolate, "__arg", v8_str(isolate, arg));*/
	/*[NpgSql....]*/
	v8::Local<v8::ObjectTemplate> npgsql_object = v8::ObjectTemplate::New(isolate);
	npgsql_object->Set(isolate, "execute_io", v8::FunctionTemplate::New(isolate, npgsql_execute_io));
	npgsql_object->Set(isolate, "execute_scalar", v8::FunctionTemplate::New(isolate, npgsql_execute_scalar));
	npgsql_object->Set(isolate, "execute_non_query", v8::FunctionTemplate::New(isolate, npgsql_execute_non_query));
	npgsql_object->Set(isolate, "data_reader", v8::FunctionTemplate::New(isolate, npgsql_data_reader));
	v8_global->Set(isolate, "npgsql", npgsql_object);
	/*[/NpgSql....]*/
	/*[pdf_generator....]*/
	v8::Local<v8::ObjectTemplate> pdf_object = v8::ObjectTemplate::New(isolate);
	pdf_object->Set(isolate, "generate_pdf", v8::FunctionTemplate::New(isolate, generate_pdf));
	v8_global->Set(isolate, "native_pdf", pdf_object);
	/*[/pdf_generator....]*/
	/*[crypto....]*/
	v8::Local<v8::ObjectTemplate> crypto_object = v8::ObjectTemplate::New(isolate);
	crypto_object->Set(isolate, "encrypt", v8::FunctionTemplate::New(isolate, encrypt_str));
	crypto_object->Set(isolate, "decrypt", v8::FunctionTemplate::New(isolate, decrypt_str));
	crypto_object->Set(isolate, "encrypt_source", v8::FunctionTemplate::New(isolate, encrypt_source));
	crypto_object->Set(isolate, "decrypt_source", v8::FunctionTemplate::New(isolate, decrypt_source));
	v8_global->Set(isolate, "crypto", crypto_object);
	/*[/crypto....]*/
	/*[http_request]*/
	v8_global->Set(isolate, "create_http_request", v8::FunctionTemplate::New(isolate, http_request));
	/*[/http_request]*/
	/*[require]*/
	v8_global->Set(isolate, "require", v8::FunctionTemplate::New(isolate, require));
	/*[/require]*/
	v8_global->Set(isolate, "hex_to_string", v8::FunctionTemplate::New(isolate, hex_to_string));
	v8_global->Set(isolate, "string_to_hex", v8::FunctionTemplate::New(isolate, string_to_hex));
	/*[__async]*/
	v8_global->Set(isolate, "__async", v8::FunctionTemplate::New(isolate, async_func));
	/*[__async]*/
	/*[__sleep]*/
	v8_global->Set(isolate, "setTimeout", v8::FunctionTemplate::New(isolate, set_time_out_func));
	v8_global->Set(isolate, "__sleep", v8::FunctionTemplate::New(isolate, sleep_func));
	/*[__sleep]*/
	v8_global->Set(isolate, "__async_t", v8::FunctionTemplate::New(isolate, async_t));
	/*[IO/file read/write....]*/
	v8::Local<v8::ObjectTemplate> io_object = v8::ObjectTemplate::New(isolate);
	io_object->Set(isolate, "read_file", v8::FunctionTemplate::New(isolate, native_read_file));
	io_object->Set(isolate, "write_file", v8::FunctionTemplate::New(isolate, native_write_file));
	v8_global->Set(isolate, "fs", io_object);
	/*[IO/file read/write....]*/
	/*[Sys Object]*/
	v8::Local<v8::ObjectTemplate> sys_object = v8::ObjectTemplate::New(isolate);
	sys_object->Set(isolate, "read_directory", v8::FunctionTemplate::New(isolate, read_directory));
	sys_object->Set(isolate, "read_directory_regx", v8::FunctionTemplate::New(isolate, read_directory_regx));
	sys_object->Set(isolate, "create_directory", v8::FunctionTemplate::New(isolate, native_create_directory));
	sys_object->Set(isolate, "delete_directory", v8::FunctionTemplate::New(isolate, native_delete_directory));
	sys_object->Set(isolate, "create_process", v8::FunctionTemplate::New(isolate, native_create_process));
	sys_object->Set(isolate, "create_child_process", v8::FunctionTemplate::New(isolate, native_create_child_process));
	sys_object->Set(isolate, "open_process", v8::FunctionTemplate::New(isolate, native_open_process));
	sys_object->Set(isolate, "terminate_process", v8::FunctionTemplate::New(isolate, native_terminate_process));
	sys_object->Set(isolate, "current_process_id", v8::FunctionTemplate::New(isolate, native_current_process_id));
	sys_object->Set(isolate, "process_is_running", v8::FunctionTemplate::New(isolate, native_process_is_running));
	sys_object->Set(isolate, "kill_process_by_name", v8::FunctionTemplate::New(isolate, native_kill_process_by_name));
	//
	v8_global->Set(isolate, "sys", sys_object);
	/*[/Sys Object]*/
	v8_global->Set(isolate, "__clear", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		//v8::Isolate* isolate = args.GetIsolate();
		//dispose_internal_object(isolate);
		delete _root_dir;
	}));
	//
	v8_global->Set(isolate, "__print", v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
		v8::Isolate* iso = args.GetIsolate();
		if (!args[0]->IsString() || args[0]->IsNullOrUndefined()) {
			iso->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(iso, "String Required!!!")));
			return;
		}
		v8::String::Utf8Value utf_msg_str(iso, args[0]);
		std::cout << *utf_msg_str << "\r\n";
		return;
	}));
	/*[Create C++ Internal Context]*/
	//v8_global->SetInternalFieldCount(1);
	return v8::Context::New(isolate, nullptr, v8_global/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
}