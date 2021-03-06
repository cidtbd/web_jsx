//2:32 AM 11/27/2018
/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "pdf_generator.h"
void pdf_ext::pdf_generator::set_status(int ret_val, const char * ret_msg) {
	_msg = new char[strlen(ret_msg) + 1];
	strcpy(_msg, ret_msg);
	_status = ret_val;
};
void pdf_ext::pdf_generator::init_func() {
	if (finished != nullptr) {
		wkhtmltopdf_set_finished_callback(_converter, finished);
	}
	if (progress_changed != nullptr) {
		wkhtmltopdf_set_progress_changed_callback(_converter, progress_changed);
	}
	if (phase_changed != nullptr) {
		wkhtmltopdf_set_phase_changed_callback(_converter, phase_changed);
	}
	if (error != nullptr) {
		wkhtmltopdf_set_error_callback(_converter, error);
	}
	if (warning != nullptr) {
		wkhtmltopdf_set_warning_callback(_converter, warning);
	}
}
int pdf_ext::pdf_generator::init_wgs() {
	if (_status < 0) {
		set_status(-1, "Not initilized. Please call `init(true);`!!!");
		return _status;
	}
	if(_prepared_wgs>0)return _status;
	for (std::map<const char*, const char*>::iterator itr = _wgs_settings->begin(); itr != _wgs_settings->end(); itr++) {
		wkhtmltopdf_set_global_setting(_wgs, itr->first, itr->second);
	}
	return _status;
};
int pdf_ext::pdf_generator::init_wos() {
	if (_status < 0) {
		set_status(-1, "Not initilized. Please call `init(true);`!!!");
		return _status;
	}
	if (_prepared_wos > 0)return _status;
	for (std::map<const char*, const char*>::iterator itr = _wos_settings->begin(); itr != _wos_settings->end(); itr++) {
		wkhtmltopdf_set_object_setting(_wos, itr->first, itr->second);
	}
	return _status;
};
int pdf_ext::pdf_generator::validate_settings(std::vector<const char*>& key, std::map<const char*, const char*>& settings) {
	return _status;
};
pdf_ext::pdf_generator::pdf_generator() {
	_status = -1;
	version = wkhtmltopdf_version();
	_prepared_wgs = -1;
	_prepared_wos = -1;
	prepare_default_settings();
};
const char * pdf_ext::pdf_generator::get_status_msg() {
	return const_cast<const char*>(_msg);
};
pdf_ext::pdf_generator::~pdf_generator() {
	if (!_disposed)dispose();
};
void pdf_ext::pdf_generator::prepare_default_settings() {
	_wgs_settings = new std::map<const char*, const char*>{
		{ "useCompression", "false" }, { "documentTitle", "Hello World" }, { "size.paperSize", "A4" },
		{ "orientation", "Portrait" }, { "colorMode", "Color" }, { "dpi", "80" }, { "imageDPI", "300" },
		{ "imageQuality", "92" }, { "margin.top", "1.27cm" }, { "margin.bottom", "1.27cm" }, 
		{ "margin.left", "1.27cm" }, { "margin.right", "1.27cm" }
	};
	malloc(sizeof _wgs_settings);
	_wos_settings = new std::map<const char*, const char*>{
		{ "web.defaultEncoding", "utf-8" }, { "web.background", "true" }, { "web.loadImages", "true" },
		{ "web.enablePlugins", "false" }, { "web.enableJavascript", "false" },
		{ "web.enableIntelligentShrinking", "true" }, { "web.minimumFontSize", "12" },
		{ "web.printMediaType", "true" }, { "header.fontSize", "8" }, { "header.fontName", "Times New Roman" },
		{ "header.left", "[date]" }, { "header.line", "false" }, { "header.spacing", "0" }, { "footer.fontSize", "8" },
		{ "footer.left", "Powered by @ https://safeonline.world" }, { "footer.right", "Page [page] of [topage]" },
		{ "footer.line", "false" }, { "footer.spacing", "0" }
	};
	malloc(sizeof _wos_settings);
};
int pdf_ext::pdf_generator::init(int use_graphics) {
	_status = -1;
	if (wkhtmltopdf_init(use_graphics) != 1) {
		//printf("Init failed");
		_disposed = true;
		set_status(_status, "PDF Engine init failed!!!");
		return -1;
	}
	_wgs = wkhtmltopdf_create_global_settings();
	_wos = wkhtmltopdf_create_object_settings();
	_status = 1;
	return _status;
};
int pdf_ext::pdf_generator::update_map_key(std::map<const char*, const char*>&header, const char*key, const char*values) {
	std::const_cmp_iter<std::map<const char*, const char*>::iterator> itr(header.begin(), header.end());
	auto it = itr.find(key);
	if (it != itr.end()) {
		it->second = values;
		return 1;
	}
	return -1;
};
int pdf_ext::pdf_generator::init(int use_graphics, std::map<const char*, const char*>& wgs_settings, std::map<const char*, const char*>& wos_settings) {
	_status = -1;
	if (wkhtmltopdf_init(use_graphics) != 1) {
		_disposed = true;
		set_status(_status, "PDF Engine init failed!!!");
		return -1;
	}
	_wgs = wkhtmltopdf_create_global_settings();
	_wos = wkhtmltopdf_create_object_settings();
	if (!wgs_settings.empty()&& !_wgs_settings->empty()) {
		for (auto itr = wgs_settings.begin(); itr != wgs_settings.end(); itr++) {
			int rec = this->update_map_key(*this->_wgs_settings, itr->first, itr->second);
			if (rec < 0) {
				std::string msg("Invlaid key defined in global_settings==>");
				msg.append(itr->first);
				set_status(_status, msg.c_str());
				std::string().swap(msg);
				return -1;
			};
		}
	}
	if (!wos_settings.empty()) {
		for (auto itr = wos_settings.begin(); itr != wos_settings.end(); itr++) {
			if (this->update_map_key(*this->_wos_settings, itr->first, itr->second) < 0) {
				std::string msg("Invlaid key defined in object_settings==>");
				msg.append(itr->first);
				set_status(_status, msg.c_str());
				std::string().swap(msg);
				return -1;
			};
		}
	}
	
	_status = 1;
	return _status;
};

long pdf_ext::pdf_generator::generate(const char * html, std::string& str_output) {
	//const char *html = "<b>foo</b>bar";
	//const char *version = wkhtmltopdf_version();
	//printf("Version: %s\n", version);
	// Init wkhtmltopdf in graphics-less mode
	if (_status < 0) {
		set_status (-1, "Not initialized!");
		return _status;
	}
	/*
	* Create a global settings object used to store options that are not
	* related to input objects, note that control of this object is parsed to
	* the converter later, which is then responsible for freeing it
	*/
	//wkhtmltopdf_global_settings* gs = wkhtmltopdf_create_global_settings();
	//wkhtmltopdf_object_settings* os = wkhtmltopdf_create_object_settings();
	/* We want the result to be storred in the file called test.pdf */
	//
	/*wkhtmltopdf_set_global_setting(_wgs, "useCompression", "false");
	wkhtmltopdf_set_global_setting(_wgs, "documentTitle", "Hello World");
	wkhtmltopdf_set_global_setting(_wgs, "size.paperSize", "A4");
	wkhtmltopdf_set_global_setting(_wgs, "orientation", "Portrait");
	wkhtmltopdf_set_global_setting(_wgs, "colorMode", "Color");//Grayscale
	wkhtmltopdf_set_global_setting(_wgs, "size.paperSize", "A4");
	//wkhtmltopdf_set_global_setting(gs, "outline", "true");
	//wkhtmltopdf_set_global_setting(gs, "outlineDepth", "4");
	//wkhtmltopdf_set_global_setting(gs, "dumpOutline", "4");
	//
	//https://wkhtmltopdf.org/libwkhtmltox/pagesettings.html#pageImageGlobal
	wkhtmltopdf_set_global_setting(_wgs, "dpi", "80");
	wkhtmltopdf_set_global_setting(_wgs, "imageDPI", "300");
	wkhtmltopdf_set_global_setting(_wgs, "imageQuality", "92");

	wkhtmltopdf_set_global_setting(_wgs, "margin.top", "1.27cm");
	wkhtmltopdf_set_global_setting(_wgs, "margin.bottom", "1.27cm");
	wkhtmltopdf_set_global_setting(_wgs, "margin.left", "1.27cm");
	wkhtmltopdf_set_global_setting(_wgs, "margin.right", "1.27cm");
	//wkhtmltopdf_set_global_setting(gs, "out", "nested_template.pdf");
	//wkhtmltopdf_set_global_setting(gs, "load.cookieJar", "myjar.jar");
	// We want to convert to convert the qstring documentation page
	wkhtmltopdf_set_object_setting(_wos, "web.defaultEncoding", "utf-8");
	wkhtmltopdf_set_object_setting(_wos, "web.background", "true");
	wkhtmltopdf_set_object_setting(_wos, "web.loadImages", "true");
	wkhtmltopdf_set_object_setting(_wos, "web.enablePlugins", "false");
	wkhtmltopdf_set_object_setting(_wos, "web.enableJavascript", "false");
	wkhtmltopdf_set_object_setting(_wos, "web.enableIntelligentShrinking", "true");
	wkhtmltopdf_set_object_setting(_wos, "web.minimumFontSize", "12");
	wkhtmltopdf_set_object_setting(_wos, "web.printMediaType", "true");
	wkhtmltopdf_set_object_setting(_wos, "header.fontSize", "8");
	//wkhtmltopdf_set_object_setting(os, "header.fontName", "Sans-Serif");
	wkhtmltopdf_set_object_setting(_wos, "header.fontName", "Times New Roman");
	wkhtmltopdf_set_object_setting(_wos, "header.left", "[date]");
	wkhtmltopdf_set_object_setting(_wos, "header.line", "false");
	wkhtmltopdf_set_object_setting(_wos, "header.spacing", "0");

	wkhtmltopdf_set_object_setting(_wos, "footer.fontSize", "8");
	//char* left = NULL;
	//to_char("Powered by @ https://safeonline.world", left);
	wkhtmltopdf_set_object_setting(_wos, "footer.left", "Powered by @ https://safeonline.world");
	wkhtmltopdf_set_object_setting(_wos, "footer.right", "Page [page] of [topage]");
	wkhtmltopdf_set_object_setting(_wos, "footer.line", "false");
	wkhtmltopdf_set_object_setting(_wos, "footer.spacing", "0");

	wkhtmltopdf_set_object_setting(_wos, "page", "http://www.cs.au.dk/~jakobt/libwkhtmltox_0.10.0_doc/pdf_8h.html");*/
	//wkhtmltopdf_set_object_setting(_wos, "page", NULL);
	init_wgs();
	init_wos();
	_converter = wkhtmltopdf_create_converter(_wgs);
	//std::cout << html;
	//wkhtmltopdf_add_object(_converter, os, html);
	wkhtmltopdf_add_object(_converter, _wos, html);
	init_func();
	// Setup callbacks
	/*
	wkhtmltopdf_set_finished_callback(_converter, finished);
	wkhtmltopdf_set_progress_changed_callback(_converter, progress_changed);
	wkhtmltopdf_set_phase_changed_callback(_converter, phase_changed);
	wkhtmltopdf_set_error_callback(_converter, error);
	wkhtmltopdf_set_warning_callback(_converter, warning);
	*/
	_disposed = false;
	// Perform the conversion
	if (!wkhtmltopdf_convert(_converter)) {
		/* Output possible http error code encountered */
		set_status (-1, "PDF Conversion failed!");
		dispose();
		return _status;
	}
	const unsigned char *data = NULL;
	unsigned long len = wkhtmltopdf_get_output(_converter, &data);
	str_output = std::string(reinterpret_cast<const char*>(data), len);
	//str_output.resize(len);
	set_status (1, "Success");
	return static_cast<int>(len);
}
int pdf_ext::pdf_generator::generate_to_path(const char * html, const char * output_path) {
	if (_status < 0) {
		set_status (-1, "Not initialized!");
		return _status;
	}
	wkhtmltopdf_set_global_setting(_wgs, "out", output_path);
	_converter = wkhtmltopdf_create_converter(_wgs);
	wkhtmltopdf_add_object(_converter, _wos, html);
	_disposed = false;
	// Perform the conversion
	if (!wkhtmltopdf_convert(_converter)) {
		/* Output possible http error code encountered */
		set_status (-1, "PDF Conversion failed!");
		dispose();
		return _status;
	}
	set_status (1, "Success");
	return wkhtmltopdf_http_error_code(_converter);
}
;
int pdf_ext::pdf_generator::generate(const char* url, const char * output_path) {
	if (_status < 0) {
		set_status (-1, "Not initialized!");
		return _status;
	}
	init_wgs(); init_wos();
	wkhtmltopdf_set_global_setting(_wgs, "out", output_path);
	wkhtmltopdf_set_object_setting(_wos, "page", url);
	_converter = wkhtmltopdf_create_converter(_wgs);
	wkhtmltopdf_add_object(_converter, _wos, NULL);
	// Setup callbacks
	init_func();
	_disposed = false;
	// Perform the conversion
	if (!wkhtmltopdf_convert(_converter)) {
		/* Output possible http error code encountered */
		set_status (-1, "PDF Conversion failed!");
		dispose();
		return _status;
	}
	set_status (1, "Success");
	return wkhtmltopdf_http_error_code(_converter);
}
;
int pdf_ext::pdf_generator::generate(const std::string html, const char* output_path) {
	if (_status < 0) {
		set_status (-1, "Not initialized!");
		return _status;
	}
	init_wgs(); init_wos();
	wkhtmltopdf_set_global_setting(_wgs, "out", output_path);
	_converter = wkhtmltopdf_create_converter(_wgs);
	wkhtmltopdf_add_object(_converter, _wos, html.c_str());
	// Setup callbacks
	init_func();
	_disposed = false;
	// Perform the conversion
	if (!wkhtmltopdf_convert(_converter)) {
		/* Output possible http error code encountered */
		set_status (-1, "PDF Conversion failed!");
		dispose();
		return _status;
	}
	set_status (1, "Success");
	return wkhtmltopdf_http_error_code(_converter);
};
int pdf_ext::pdf_generator::generate_from_url(const char * url, std::string& str_output) {
	if (_status < 0) {
		set_status (-1, "Not initialized!");
		return _status;
	}
	init_wgs(); init_wos();
	wkhtmltopdf_set_object_setting(_wos, "page", url);
	_converter = wkhtmltopdf_create_converter(_wgs);
	wkhtmltopdf_add_object(_converter, _wos, NULL);
	// Setup callbacks
	init_func();
	_disposed = false;
	// Perform the conversion
	if (!wkhtmltopdf_convert(_converter)) {
		/* Output possible http error code encountered */
		set_status (-1, "PDF Conversion failed!");
		dispose();
		return _status;
	}
	const unsigned char *data = NULL;
	unsigned long len = wkhtmltopdf_get_output(_converter, &data);
	str_output = std::string(reinterpret_cast<const char*>(data), len);
	set_status (1, "Success");
	return static_cast<int>(len);
};
int pdf_ext::pdf_generator::generate_from_url(const char * url, const char* output_path) {
	if (_status < 0) {
		set_status (-1, "Not initialized!");
		return _status;
	}
	init_wgs(); init_wos();
	wkhtmltopdf_set_global_setting(_wgs, "out", output_path);
	wkhtmltopdf_set_object_setting(_wos, "page", url);
	_converter = wkhtmltopdf_create_converter(_wgs);
	wkhtmltopdf_add_object(_converter, _wos, NULL);
	_disposed = false;
	// Perform the conversion
	if (!wkhtmltopdf_convert(_converter)) {
		/* Output possible http error code encountered */
		set_status (-1, "PDF Conversion failed!");
		dispose();
		return _status;
	}
	set_status (1, "Success");
	return wkhtmltopdf_http_error_code(_converter);
};
void pdf_ext::pdf_generator::global_settings(std::map<const char*, const char*>& settings) {
};
void pdf_ext::pdf_generator::object_settings(std::map<const char*, const char*>& settings) {
};

void pdf_ext::pdf_generator::dispose() {
	if (!_disposed) {
		delete _msg;
		_disposed = true;
		if (_converter != NULL) {
			/* Destroy the converter object since we are done with it */
			wkhtmltopdf_destroy_converter(_converter);
			/* We will no longer be needing wkhtmltoimage funcionality */
			wkhtmltopdf_deinit();
		}
		delete _wgs_settings; delete _wos_settings;
	}
};
/*void finished(wkhtmltopdf_converter* converter, int p) {
	//printf("Finished: %d\n", p);
};

void progress_changed(wkhtmltopdf_converter* converter, int p) {
	//printf("%3d\n", p);
};

void phase_changed(wkhtmltopdf_converter* converter) {
	//int phase = wkhtmltopdf_current_phase(converter);
	//printf("Phase: %s\n", wkhtmltopdf_phase_description(converter, phase));
};

void error(wkhtmltopdf_converter* converter, const char * msg) {
	//printf("Error: %s\n", msg);
};

void warning(wkhtmltopdf_converter* converter, const char * msg) {
	//printf("Warning: %s\n", msg);
};*/
/**void to_char(const char* const_str, char*out_str) {
	out_str = new char[strlen(const_str)];
	strcpy(out_str, const_str);
};*/