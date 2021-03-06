/**
* Copyright (c) 2018, SOW (https://www.safeonline.world). (https://github.com/RKTUXYN) All rights reserved.
* @author {SOW}
* Copyrights licensed under the New BSD License.
* See the accompanying LICENSE file for terms.
*/
#include "directory_.h"
//3:21 PM 12/24/2018
bool __is_match_extension_x (const std::string& path_str, const std::regex & pattern) {
	return std::regex__ismatch(path_str, pattern);
};
bool __is_match_extension (const std::string& path_str, const char* ext) {
	size_t found = path_str.find_last_of(".");
	if (found == std::string::npos)
		return false;
	return strcmp(path_str.substr(found + 1).c_str(), ext) == 0;
};
#if !defined(_WINDOWS_)
int sow_web_jsx::read_directory_files(const char* name, std::vector<char*>&directory) {
#error Not Implemented
};
int sow_web_jsx::read_directory_sub_directory(const char* name, std::vector<const char*>&directory, const char* ext) {
#error Not Implemented
};
int sow_web_jsx::read_directory_sub_directory(const char* name, std::vector<std::string>&directory, const char* ext) {
#error Not Implemented
};
int sow_web_jsx::dir_exists(const char* dir) {
	struct stat stats;
	stat(dir, &stats);
	// Check for file existence
	if (S_ISDIR(stats.st_mode))
		return 1;
	return 0;
};
int sow_web_jsx::create_directory(const char* dir) {
#error Not Implemented
};
int sow_web_jsx::delete_dir(const char * name) {
#error Not Implemented
};
int sow_web_jsx::read_directory_sub_directory_x(const char * name, std::vector<std::string>& directory, const std::regex& pattern) {
#error Not Implemented
};
#else
int sow_web_jsx::read_directory_files(const char* name, std::vector<char*>&directory) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	struct dirent *entry;
	/* read all the files and directories within directory */
	while ((entry = readdir (dir)) != NULL) {
		if (entry->d_type == DT_DIR) continue;
		char path[_MAX_PATH];
		snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
		directory.push_back(path);
	}
	closedir (dir);
	return EXIT_SUCCESS;
};
int sow_web_jsx::read_directory_sub_directory(const char* name, std::vector<const char*>&directory, const char* ext) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	bool has_ext = strcmp(ext, "A") != 0;
	struct dirent *entry;
	while ((entry = readdir (dir)) != NULL) {
		if (entry->d_type == DT_DIR) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			char* path =(char *)malloc(_MAX_PATH);
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			read_directory_sub_directory(const_cast<const char*>(path), directory, ext);
			free(path);
			continue;
		};
		if (has_ext) {
			if (__is_match_extension(entry->d_name, ext) == false)continue;
		}
		char* path = (char *)malloc(_MAX_PATH);
		snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
		directory.push_back(const_cast<const char*>(path));
		free(path);
	}
	closedir (dir);
	return EXIT_SUCCESS;
};
int sow_web_jsx::read_directory_sub_directory(const char* name, std::vector<std::string>&directory, const char* ext) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	bool has_ext = strcmp(ext, "A") != 0;
	struct dirent *entry;
	while ((entry = readdir (dir)) != NULL) {
		const char* d_name = const_cast<const char*>(entry->d_name);
		if (entry->d_type == DT_DIR) {
			if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0)
				continue;
			std::string path(name);
			path.append(d_name);
			path.append("\\");
			read_directory_sub_directory(path.c_str(), directory, ext);
			std::string().swap(path);
			continue;
		};
		if (has_ext) {
			if (__is_match_extension(d_name, ext) == false)continue;
		}
		std::string path(name);
		path.append(d_name);
		directory.push_back(path);
		std::string().swap(path);
	}
	closedir (dir);
	return EXIT_SUCCESS;
};
int sow_web_jsx::dir_exists(const char* dir) {
	wchar_t* wString = (wchar_t*)malloc(MAX_PATH);
	MultiByteToWideChar(CP_ACP, 0, dir, -1, wString, MAX_PATH);
	if (CreateDirectory(wString, NULL)) {
		free(wString);
		return 1;
	}
	if (ERROR_ALREADY_EXISTS == GetLastError()) {
		free(wString);
		return -1;
	}
	free(wString);
	return -3;
};
int sow_web_jsx::create_directory(const char* dir) {
	DIR* d = opendir(dir);
	if (d != NULL) {
		closedir(d);
		return -1;
	}
	if (_mkdir(dir) == 0)
		return 2;
	return -2;
};
int sow_web_jsx::delete_dir(const char * name) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	struct dirent *entry;
	int error = 0;
	while ((entry = read_dir (dir)) != NULL) {
		const char* d_name = const_cast<const char*>(entry->d_name);
		if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0)
			continue;
		std::string d_path(name);
		d_path.append(d_name);
		d_path.append("\\");
		error = sow_web_jsx::delete_dir(d_path.c_str());
		std::string().swap(d_path);
		if (error == EXIT_FAILURE)break;
	};
	closedir (dir);
	if (error == EXIT_FAILURE)return EXIT_FAILURE;
	if (_rmdir(name) == 0)
		return EXIT_SUCCESS;
	return EXIT_FAILURE;
};
int sow_web_jsx::read_directory_sub_directory_x(const char * name, std::vector<std::string>& directory, const std::regex& pattern) {
	DIR *dir;
	if (!(dir = opendir(name)))
		/* could not open directory */
		return EXIT_FAILURE;
	
	struct dirent *entry;
	while ((entry = readdir (dir)) != NULL) {
		if (entry->d_type == DT_UNKNOWN)continue;
		const char* d_name = const_cast<const char*>(entry->d_name);
		if (entry->d_type == DT_DIR) {
			if (strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0)
				continue;
			std::string path(name);
			path.append(d_name);
			path.append("\\");
			read_directory_sub_directory_x(path.c_str(), directory, pattern);
			std::string().swap(path);
			continue;
		};
		if (__is_match_extension_x(d_name, pattern) == false)continue;
		std::string path(name);
		path.append(d_name);
		directory.push_back(path);
		std::string().swap(path);
	}
	closedir (dir);
	return EXIT_SUCCESS;
};
#endif//!_WINDOWS_