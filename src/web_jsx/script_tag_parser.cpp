/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "script_tag_parser.h"
using namespace sow_web_jsx;
void add_common_func(std::stringstream& stream) {
	stream << "String.format = function ( format ) {if ( typeof ( format ) !== 'string' ) throw new Error(\"String required!!!\"); let args = Array.prototype.slice.call( arguments, 1 ); let len = args.length - 1; return format.replace( /{(\\d+)}/g, function ( match, number )  { let index = parseInt( number ); if ( isNaN( index ) ) throw new Error( \"Invalid param index!!!\" ); if ( index > len ) throw new Error( \"Index should not greater than \" + len + format + JSON.stringify( args ) ); return typeof ( args[index] ) !== 'undefined' ? args[number] : \"\"; } ); };\n";
};
jsx_export void sow_web_jsx::js_write_header(std::stringstream& stream) {
	stream << "context.app_path = ( function () { if ( context.app_path === undefined || context.app_path === null ) { throw new Error(\"App path not found in current context!!!\"); } return context.app_path.replace( /\\\\/g,'/' ) }());\n";
	stream << "context.env_path = ( function () { if ( context.env_path === undefined || context.env_path === null ) { throw new Error(\"Environment path not found in current context!!!\"); } return context.env_path.replace( /\\\\/g,'/' ) }());\n";
	stream << "context.root_dir = (function () { if ( context.root_dir === undefined || context.root_dir === null ) { throw \"Root directory not found in current context!!!\"; } return context.root_dir.replace( /\\\\/g,'/' ) }());\n";
	stream << "context.https = context.https===\"on\" || context.https===\"true\" ? true : false;\n";
	stream << "context.request.protocol= context.https===true ? \"https:\" : \"http:\";\n";
	stream << "context.host_url = context.request.protocol + \"//\" + context.host;\n";
	stream << "context.request.query_string = ( function () { try { return JSON.parse( (context.request.query_string) ); } catch ( e ) { return {}; } }() );\n";
	stream << "context.request.content_length = (function(){ let len = parseInt(context.request.content_length); return isNaN(len) ? 0 : len; }())\r\n";
	stream << "context.request.read_payload = function read_payload( cb ) { return this.method !== 'POST'? -1 : this._read_payload(this.content_length, cb);}\r\n";
	stream << "context.response.write = function write( val, nline ) { if ( !val || val === null || val === undefined ) return this;  !1 === nline ? context.response._write( val ) : context.response._write( val + '\\r\\n' ); return this;};\r\n";
	stream << "context.response.write_p = function write_p( val ) { if ( !val || val === null || val === undefined ) return this; context.response._write( val ); return this;};\r\n";
	stream << "context.response.redirect = function redirect( url, force ) { if ( !url ) { throw \"Redirect location required!!!\"; } context.response._redirect( context.server_protocol, ( force === true ? ( context.https ? \"https://\" : \"http://\" ) + ( context.host + url.trim() ) : url.trim() ) ); };\r\n";
	stream << "context.response.status(context.server_protocol, \"200\", \"OK\");\n";
	stream << "context.response.as_gzip = function as_gzip() { if ( !context.request.accept_encoding ) return -1; if ( context.request.accept_encoding.indexOf( 'gzip' ) < 0 ) return -1; this._as_gzip(); return 1; };\n";
	stream << "context.response.header(\"Content-Type\", \"text/html; charset=utf-8\");\n";
	stream << "context.response.header(\"Connection\", \"Keep-Alive\");\n";
	stream << "fs.write_file_from_payload = function write_file_from_payload( path ) { if ( context.request.method !== 'POST' ) throw new Error('Allowed only over POST data.'); return this._write_file_from_payload( context.request.content_length, path ); }\r\n";
	add_common_func(stream);
	stream << "try{\n";
};
jsx_export void sow_web_jsx::js_write_footer(std::string&str) {
	str.append("\ncontext.response.header(\"X-Powered-By\", \"safeonline.world\");\n");
	str.append("context.response.header(\"X-Process-By\", \"web_jsx_cgi\");\n");
	str.append("}catch(_exp){\n\
				context.response.clear();\n\
				context.response.header(\"Content-Type\", \"text/html\");\n\
				if(typeof(_exp)!==\"object\"){\n\
					context.response.write('Error::' + _exp );\n\
				}else{\n\
					context.response.write('Error::' + _exp.message + '<br/>' );\n\
					context.response.write(_exp.stack );\n\
				}\n\
				/*context.response.body.flush();*/\n\
			};");
};
jsx_export void sow_web_jsx::js_write_footer(std::stringstream& stream) {
	stream << "\ncontext.response.header(\"X-Powered-By\", \"safeonline.world\");\n";
	stream << "context.response.header(\"X-Process-By\", \"web_jsx_cgi\");\n";
	stream << "}catch(_exp){\n";
	stream << "context.response.clear();\n";
	stream << "context.response.header(\"Content-Type\", \"text/html\");\n";
	stream << "if(typeof(_exp)!==\"object\"){\n";
	stream << "context.response.write('Error::' + _exp );\n";
	stream << "}else{\n";
	stream << "context.response.write('Error::' + _exp.message + '<br/>' );\n";
	stream << "context.response.write(_exp.stack );\n";
	stream << "}\n";
	stream << "};";
};
jsx_export void sow_web_jsx::js_write_console_header(std::stringstream& stream) {
	stream << "env.app_dir = ( function () { if ( env.app_dir === undefined || env.app_dir === null ) { throw new Error(\"App directory not found in current context!!!\"); } return env.app_dir.replace( /\\\\/g,'/' ).replace(/\\/\\//gi, '/'); }());\n";
	stream << "env.app_path = ( function () { if ( env.app_path === undefined || env.app_path === null ) { throw new Error(\"App path not found in current context!!!\"); } return env.app_dir + env.app_path.replace( /\\\\/g,'/' )+'.exe'; }());\n";
	stream << "env.root_dir = ( function () { if ( env.root_dir === undefined || env.root_dir === null ) { throw new Error(\"Root directory not found in current context!!!\"); } return env.root_dir.replace( /\\\\/g,'/' ) }());\n";
	stream << "env.path_translated = ( function () { if ( env.path_translated === undefined || env.path_translated === null ) { throw new Error(\"Translated path not found in current context!!!\"); } return env.path_translated.replace( /\\\\/g,'/' ) }());\n";
	stream << "env.path = ( function () { if ( env.path === undefined || env.path === null ) { throw new Error(\"Environment path not found in current context!!!\"); } return env.path.replace( /\\\\/g,'/' ) }());\n";
	stream << "env.arg = ( function () { try { return JSON.parse( env.arg ); } catch ( e ) { __print( e.message ); return []; } }() );\n";
	add_common_func(stream);
	stream << "try{\n";
};
jsx_export void sow_web_jsx::js_write_console_footer(std::stringstream& stream) {
	stream << "\n}catch(_exp){\n";
	stream << "if(typeof( _exp )!==\"object\"){\n";
	stream << "__print('Error::' + _exp );\n";
	stream << "}else{\n";
	stream << "__print('Error::' + _exp.message );\n";
	stream << "__print( _exp.stack );\n";
	stream << "}\n";
	stream << "};\n";
};
typedef enum {
	sts,
	ste,
	rts,
	rte
}e_tag;
typedef struct {
	std::string line, tag, start_tage_name;
	bool is_tag_start, is_tag_end, is_last_tag, tag_found;
	e_tag etag;
}parser_info;
typedef struct {
	std::string sts/*sts*/ = "<js::";
	std::string ste/*ste*/ = "::js>";
	std::string rts/*rts*/ = "<js=";
	std::string rte/*rte*/ = "=js>";
}script_tag;
class js_parser {
public:
	js_parser() {
		script_tag *tags = new script_tag();
		this->regx_sts = new std::regex("(" + tags->sts + ")");
		this->regx_ste = new std::regex("(" + tags->ste + ")");
		this->regx_rts = new std::regex("(" + tags->rts + ")");
		this->regx_rte = new std::regex("(" + tags->rte + ")");
		this->rep_str = "88_REP__STR__88";
		this->rep_str_regex = new std::regex("(" + this->rep_str + ")");
		delete tags;
	}
	~js_parser() {
		delete this->regx_sts;
		delete this->regx_ste;
		delete this->regx_rts;
		delete this->regx_rte;
		delete this->rep_str_regex;
		std::string().swap(this->rep_str);
	};
private:
public:
	std::regex* regx_sts;
	std::regex* regx_ste;
	std::regex* regx_rts;
	std::regex* regx_rte;
	std::string rep_str;
	std::regex* rep_str_regex;
	int start_tag(const std::regex regx, parser_info&info) {
		if (REGEX_IS_MATCH(info.line, regx) == 0) {
			if (info.is_tag_end && info.etag != e_tag::sts) {
				info.line = info.line += " " + this->rep_str + "); context.response.write(" + this->rep_str;
			}
			return 0;
		}
		bool _default = true;
		info.is_tag_start = true;
		switch (info.etag) {
		case e_tag::sts/*<js::*/:
			if (REGEX_IS_MATCH(info.line, *this->regx_ste) == 0) {
				info.is_tag_end = false;
				info.line = std::regex_replace(info.line, regx, " " + this->rep_str + ");");
				break;
			}
			info.is_tag_end = true;
			info.is_tag_start = false;
			//(?:<js::.+?::js>)
			info.line = REGEX_REPLACE_MATCH(info.line, std::regex("(<js::(.+?)::js>)"),
				[&](const std::smatch& m) {
				std::string matchstr(m.str(0));
				if (matchstr.empty()) {
					return std::string("");
				}
				//<js::context.response.write('JS Object==>');::js> BUG
				//<js:: context.response.write("JS Object==>");::js> WORK
				matchstr = std::regex_replace(matchstr, *this->regx_sts, this->rep_str + ", true);\r\n");
				matchstr = std::regex_replace(matchstr, *this->regx_ste, "\r\ncontext.response.write(" + this->rep_str);
				return matchstr;
			});
			break;
		case e_tag::rts/*<js=*/:
			if (REGEX_IS_MATCH(info.line, *this->regx_rte) == 0) {
				info.is_tag_end = false;
				info.line = std::regex_replace(info.line, regx, "");
				info.line.append(this->rep_str + ");\r\ncontext.response.write(");
				break;
			}
			info.is_tag_end = true;
			info.is_tag_start = false;
			//(?:<js=.+?=js>)
			info.line = REGEX_REPLACE_MATCH(info.line, std::regex("(<js=(.+?)=js>)"),
				[&](const std::smatch& m) {
				std::string matchstr(m.str(0));
				if (matchstr.empty()) {
					return std::string("");
				}
				matchstr = std::regex_replace(matchstr, *this->regx_rts, this->rep_str + ", !1).write_p(");
				matchstr = std::regex_replace(matchstr, *this->regx_rte, ").write(" + this->rep_str);
				return matchstr;
			});
			break;
		default: _default = false; break;
		}
		if (_default == false) {
			throw new std::runtime_error("We got error, Invalid script tag found... :(");
		}
		return 0;
	};
	int end_tag(const std::regex regx, parser_info&info) {
		if (info.is_tag_start == false && info.is_tag_end == true) {
			return 0;
		}
		if (info.is_tag_start != false && info.is_tag_end != true) {
			/** Check End Tag**/
			info.is_tag_start = true;
			if (REGEX_IS_MATCH(info.line, regx) == 0) {
				info.is_tag_end = false;
				return -1;
			}
			switch (info.etag) {
			case e_tag::ste/*::js>*/:
				info.is_tag_end = true;
				info.is_tag_start = false;
				info.line = std::regex_replace(info.line, regx, "\r\ncontext.response.write(" + this->rep_str);
				break;
			case e_tag::rte/*=js>*/:
				info.is_tag_end = true;
				info.is_tag_start = false;
				info.line = std::regex_replace(info.line, regx, ");\r\ncontext.response.write(" + this->rep_str);
				break;
			default:break;
			}
			return 0;
		}
		return 0;
	};
};
int script_tag_parser::parse(template_result & tr, std::string&html_body) {
	std::regex*regx_sts = new std::regex("(<js::)");
	std::regex*regx_str = new std::regex("(<js=)");
	if (REGEX_IS_MATCH(html_body, *regx_sts) == 0) {
		delete regx_sts;
		if (REGEX_IS_MATCH(html_body, *regx_str) == 0) {
			if (tr.remove_new_line) {
				//Normalize
				html_body = std::regex_replace (html_body, std::regex("\\n\\s*\\n"), "\r\n");
			}
			tr.is_script_template = false;
			return 0;
		}
		delete regx_str;
	}
	else {
		delete regx_sts;
		delete regx_str;
	}
	tr.is_script_template = true;
	std::stringstream stream(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	sow_web_jsx::js_write_header(stream);
	std::regex* rgx = new std::regex("(?:\\r\\n|\\r|\\n)");
	std::sregex_token_iterator iter(html_body.begin(), html_body.end(), *rgx, -1);
	std::regex* qu = new std::regex("'");
	parser_info* info = new parser_info();
	js_parser* jsp = new js_parser();
	info->is_tag_start = false;
	info->is_tag_end = true;
	for (std::sregex_token_iterator end; iter != end; ++iter) {
		info->tag_found = false;
		info->line = iter->str();
		if (info->line.empty()) {
			continue;
		}
		if (info->is_tag_end) {
			info->line = "context.response.write(" + jsp->rep_str + info->line;
			stream << "\n";
		}
		info->etag = e_tag::sts;
		jsp->start_tag(*jsp->regx_sts, *info);/**TAG-1 <js::**/
		info->etag = e_tag::ste;
		if (jsp->end_tag(*jsp->regx_ste, *info)/**TAG-2 ::js>**/ < 0)
			stream << "\n";
		info->etag = e_tag::rts;
		jsp->start_tag(*jsp->regx_rts, *info);/**TAG-3 <js=**/
		info->etag = e_tag::rte;
		jsp->end_tag(*jsp->regx_rte, *info);/**TAG-4 =js>**/
		if (info->is_tag_end) {
			info->line = std::regex_replace(info->line, *qu, "\\x27");
			info->line = std::regex_replace(info->line, *jsp->rep_str_regex, "'");
			stream << info->line << "');";
			std::string().swap(info->line);
			continue;
		}
		info->line = std::regex_replace(info->line, *jsp->rep_str_regex, "'");
		stream << info->line;
		std::string().swap(info->line);
	}
	std::string().swap(html_body);
	delete info; delete rgx;
	delete qu; delete jsp;
	html_body = stream.str();
	std::stringstream().swap(stream);
	html_body = std::regex_replace(html_body, std::regex("(?:context.response.write\\(''\\);)"), "");
	html_body = std::regex_replace(html_body, std::regex("(?:context.response.write\\(' '\\);)"), "");
	sow_web_jsx::js_write_footer(html_body);
	return 1;
};
