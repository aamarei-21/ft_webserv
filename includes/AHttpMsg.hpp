#pragma once

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include "utils.hpp"
#include "ConfigBlock.hpp"

using std::string;

class AHttpMsg {
public:
    string raw;
    string version;
    std::map<string, u_int32_t, Comparator> indexes;
    std::vector<std::pair<string, string> > tokens;
    string msg_body;
    string sep;  // boundary separator
    string empty;

    AHttpMsg(): raw(""), version("HTTP/1.1"), indexes(), tokens(),
                msg_body(), sep(), empty() {}

    AHttpMsg(AHttpMsg const &oth) {
        *this = oth;
    }

    AHttpMsg& operator=(AHttpMsg const &oth) {
        if (this != &oth) {
            raw = oth.raw;
            version = oth.version;
            indexes = oth.indexes;
            tokens = oth.tokens;
            msg_body = oth.msg_body;
            sep = oth.sep; 
        }
        return *this;
    }

    virtual ~AHttpMsg() {}

    string get_raw() const { return raw; }
    string get_version() const { return version; }
    std::map<string, string > get_headers() const;
    string & get_msg_body() { return msg_body; }
    u_int32_t get_headers_size() const { return indexes.size(); }

    string const& get_header(const string &str) const {
        try {
            return tokens[indexes.at(str)].second;
        } catch (const std::out_of_range &) {}
        return empty;
    }

    bool headers_has(const string &str) {
        try {
            indexes.at(str);
        } catch (const std::out_of_range &) {
            return false;
        }
        return true;
    }

    string & operator[](const string &str) {
        std::map<string, u_int32_t>::const_iterator it = indexes.find(str);
        if (it != indexes.end())
            return (tokens[it->second].second);
        else
            throw std::invalid_argument("Header not found");
    }

    string const & operator[](const string &str) const {
        try {
            return tokens[indexes.at(str)].second;
        } catch (const std::out_of_range &ex) {
            std::cerr << "Error: Header not found " << ex.what() << '\n';
        }
        return empty;
    }

    const string operator[](u_int32_t index) const {
        if (index < indexes.size())
            return tokens[index].first + ": " + tokens[index].second;
        return std::string();
    }

    void set_version(const string &v) { version = v; }
    void set_body(const string &b) { msg_body = b; }
    void body_append(const string &b) { msg_body += b; }
    void set_header(const string &first, const string &second);
    int add_header(const string &first, const string &second);
    int add_header(const string &first, const long long int &second);
    int add_header(const string &first, const double &second);

    virtual string to_str() const = 0;

    void add_value_in_header(u_int32_t index, const string &str);
    virtual int add_to_header(const string &first, const string &second, u_int32_t &index) = 0;

};