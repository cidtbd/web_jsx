/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
//6:24 PM 1/22/2019
#include "v8_util.h"
const char * sow_web_jsx::_toCharStr(const v8::String::Utf8Value & value) {
	if (value.length() <= 0)return "";
	return *value ? *value : "<string conversion failed>";
};
const char* sow_web_jsx::to_char_str(v8::Isolate* isolate, v8::Local<v8::Value> x) {
	v8::String::Utf8Value str(isolate, x);
	return _toCharStr(str);
};
v8::Local<v8::String> sow_web_jsx::v8_str(v8::Isolate* isolate, const char* x) {
	return v8::String::NewFromUtf8(isolate, x, v8::NewStringType::kNormal).ToLocalChecked();
}
void sow_web_jsx::set__exception(v8::Isolate * isolate, v8::TryCatch * try_catch, template_result & tr) {
	tr.is_error = true;
	v8::String::Utf8Value exception(isolate, try_catch->Exception());
	tr.err_msg = _toCharStr(exception);
}
void sow_web_jsx::get_server_map_path(const char * req_path, std::string & output) {
	output.append(req_path);
	output = std::regex_replace(output, std::regex("(?:/)"), "\\");
};
v8::Local<v8::String> sow_web_jsx::concat_msg(v8::Isolate * isolate, const char * a, const char * b) {
	char *msg = (char*)malloc(strlen(a) + strlen(b));
	sprintf(msg, "%s%s", a, b);
	v8::Local<v8::String> val = sow_web_jsx::v8_str(isolate, const_cast<const char*>(msg));
	free(msg);
	return val;
};
v8::Handle<v8::Object> sow_web_jsx::native_write_filei(v8::Isolate* isolate, const std::string abs_path, const char* buffer) {
	FILE*fstream;
	errno_t err;
	err = fopen_s(&fstream, abs_path.c_str(), "w+");
	v8::Handle<v8::Object> v8_result = v8::Object::New(isolate);
	if (err != 0) {
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			sow_web_jsx::concat_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path.c_str())
		);
		return v8_result;
	}
	size_t len = fwrite (buffer, sizeof(char), strlen(buffer), fstream);
	if (ferror(fstream)) {
		fclose(fstream);
		fstream = NULL;
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "staus_code"),
			v8::Number::New(isolate, -1)
		);
		v8_result->Set(
			v8::String::NewFromUtf8(isolate, "message"),
			sow_web_jsx::concat_msg(isolate, "Unable to create file!!! Server absolute path==>", abs_path.c_str())
		);
		return v8_result;
	}
	fclose (fstream);
	fstream = NULL;
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "staus_code"),
		v8::Number::New(isolate, static_cast<double>(len))
	);
	v8_result->Set(
		v8::String::NewFromUtf8(isolate, "message"),
		v8::String::NewFromUtf8(isolate, "Success...")
	);
	return v8_result;
};
v8::Local<v8::Context> sow_web_jsx::create_internal_context(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> v8_object) {
	internal_global_ctx* js_ctx = new internal_global_ctx();
	js_ctx->is_flush = 0;
	js_ctx->headers = new std::map<std::string, std::string>();
	js_ctx->cookies = new std::vector<std::string>();
	js_ctx->http_status = new std::vector<std::string>();
	js_ctx->root_dir = new std::string("");
	js_ctx->body_stream = std::stringstream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	v8_object->SetInternalFieldCount(1);
	v8::Local<v8::Context> ctx = v8::Context::New(isolate, nullptr, v8_object/*v8::MaybeLocal<v8::ObjectTemplate>()*/);
	v8::Local<v8::Object> global = ctx->Global();
	global->SetInternalField(0, v8::External::New(isolate, js_ctx));
	return ctx;
};

