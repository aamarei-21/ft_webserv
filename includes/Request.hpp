#pragma once

#include "AHttpMsg.hpp"
#include "utils.hpp"

class Request: public AHttpMsg {
public:
    Request(const string &headers): AHttpMsg(), method("GET"), method_e(GET), uri("/"), end(false),
                url(), params(), read_line(true), size_chanks(0), headers(headers) {}

    void parse(string &raw);
    void parse2(const string &src);

    ~Request() {}

    string get_method() const { return method; }
    int get_method_e() const { return method_e; }
    string get_uri() const { return urlDecode(uri); }
    string get_url() const { return urlDecode(url); }
    string get_params() const { return params; }

    bool get_end() const { return end; }

    void set_method(const string &m) {
        if (!check_method(m))
            throw std::invalid_argument("Start Line method error");
        this->method = m;
        std::map<string, int> convert;
        convert["GET"] = GET;
        convert["PUT"] = PUT;
        convert["POST"] = POST;
        convert["DELETE"] = DELETE;
        convert["HEAD"] = HEAD;
        convert["CONNECT"] = CONNECT;
        this->method_e = convert[m];
    }

    void set_uri(const string &u) { 
        this->uri = urlEncode(u);
        std::pair<string, string> ur = get_pair(u, '?');
        this->url = ur.first;
        this->params = ur.second;
    }
    void set_version(const string &v) { this->version = v; }
    void set_end(bool flag) { this->end = flag; }

    string to_str() const;

    void print_request() {
        std::cout << get_cur_date() << ": ";
        std::cout << this->method << " " << this->uri << " " << this->version << "\n";
        std::cout << "\t\tHost: " << this->get_header("Host") << "\n";
    }

private:
    string method;
    int method_e;
    string uri;
    bool end;
    string url;
    string params;
    bool read_line;
    uint64_t size_chanks;
    const string &headers;
    // bool read_headers;

    // Request(): AHttpMsg(), headers("") {}

    int check_method(const string &m);


    int add_to_header(const string &first, const string &second, u_int32_t &index);
};