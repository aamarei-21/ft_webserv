#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <iterator>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include "MD5.h"

using std::string;

template <typename T>
string ToString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

template <typename T>
T ToNumber ( const string &Text, int base = 10 )
{
    std::stringstream ss;
    T result;
    if (base == 16)
        ss << std::hex << Text;
    else
        ss << Text;
    return (ss >> result) ? result : 0;
}


string ToLower ( const string &Text );

std::vector<string> split(const string& s, char delimiter);
std::vector<string> split(const string& s, const string &delimiter);
string trim(const string& str, const string& whitespace = " \t\r");
string reduce(const string& str, const string& fill = " ",
            const string& whitespace = " \t\r");
std::pair<string, string> get_pair(const string& s, char delimiter);
u_int32_t count(const string& s, char f);
string find_lower(const string &header, const string &headers);

string urlEncode(const string &str);
string urlDecode(const string &str);

bool str_cmp(const string &str1, const string &str2);
bool exists(const string &name);
string read_file(const string &path);

string get_cur_date();
string get_expires_date(int min, int hours = 0, int days = 0);
string get_last_modif(const string &filename);
string get_file_size(const string &filename);
string get_hash(const string &s1);

string fix_length(size_t len, const string &str);

void free_array(char **arr);
void nonblock(int sockfd);

struct Comparator {
    bool operator () (const string & s1, const string & s2) const {
        return ToLower(s1) < ToLower(s2);
    }
};

void add_value_in_header(string const &key, std::map<string, string, Comparator> &headers, const string &str);
int add_to_header(std::map<string, string, Comparator> &headers, const string &first, const string &second, string &last_header);