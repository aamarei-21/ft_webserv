#pragma once

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <set>
#include <sys/stat.h>
#include <dirent.h>
#include "utils.hpp"

using std::string;

string autoindex(const string &path, const string &url);

string gen_error_page(const string &code, const string &status);

string gen_success_page(const string &code);