#pragma once

#include "AHttpMsg.hpp"

class Response: public AHttpMsg {
public:
    u_int32_t code;
    string path;
    
    Response(std::map<int, string> &status, const string &headers): AHttpMsg(), code(200), path(), 
                                    status_info(status), cookies(), headers(headers) {
        this->add_header("Server", "WebServ/1.0 (Mac OS)");
    }

    Response(Response const &oth): AHttpMsg(oth), code(oth.code), path(oth.path), status_info(oth.status_info),
                                    cookies(oth.cookies), headers(oth.headers)  {}

    Response& operator=(Response const &oth) {
        if (this != &oth) {
            code = oth.code;
            path = oth.path;
        }
        return *this;
    }

    void parse(const string &src) {
        (void)src;
    }

    ~Response() {}

    string to_str() const {
        string response = this->version + " " + ToString(this->code);
        response += " " + get_status() + "\r\n";
        for (u_int32_t i = 0; i < this->indexes.size(); ++i) {
            response += this->tokens[i].first + ": " + this->tokens[i].second + "\r\n";
        }
        for (u_int32_t i = 0; i < this->cookies.size(); ++i) {
            response += "Set-Cookie: " + this->cookies[i] + "\r\n";
        }
        response += "\r\n";
        response += this->msg_body;
        //response += "\r\n"; // Возможно лишнее
        return response;
    }

    void print_response(const std::string &msg="") {
        std::cout << get_cur_date() << ": ";
        std::cout << msg << "response status code " << this->code << " " << get_status() << "\n";
    }

    const string& get_status() const {
        try {
            return status_info.at(this->code);
        } catch (const std::out_of_range &ex) {
            std::cerr << "Error status code: " << ex.what() << '\n';
        }
        return this->empty;
    }

    void set_cookie(const string &value) {
        cookies.push_back(value);
    }

private:
    int add_to_header(const string &first, const string &second, u_int32_t &index) {
        std::vector<string> for_search = split(headers, ' ');
        string key;
        for (size_t i = 0; i < for_search.size(); ++i) {
            if (ToLower(first) == ToLower(for_search[i]))
                key = for_search[i];
        }
        if (key.empty())
            return 0;

        std::map<string, u_int32_t>::const_iterator it = this->indexes.find(key);
        if (it != this->indexes.end()) {
            index = it->second;
            add_value_in_header(index, reduce(second));
        } else {
            index = this->indexes.size();
            this->indexes[key] = index;
            this->tokens.push_back(std::make_pair(key, reduce(second)));
        }
        return 1;
    }
    std::map<int, string> &status_info;
    std::vector<string> cookies;
    const string &headers;
};