#pragma once

#include <iostream>
#include <map>
#include "Request.hpp"
#include "Response.hpp"
#include "utils.hpp"

class Cache
{
    public:
    Response *last_response;
    Cache() {}
    ~Cache() {
        std::map<string, Response *>::const_iterator it = data.begin();
        for ( ; it != data.end(); ++it) {
            delete it->second;
        }
        data.clear();
    }

    void add_response(Request *req, Response *resp) {
        std::string str = req->get_method() + req->get_url();
        str += req->get_params() + req->get_header("Host");
        if (req->headers_has("Accept-Encoding")
            && req->get_header("Accept-Encoding").find("gzip") != string::npos)
            str += "gzip";
        data[get_hash(str)] = resp;
        last_response = resp;
    }

    Response* get_response(Request *req) {
        std::string str = req->get_method() + req->get_url();
        str += req->get_params() + req->get_header("Host");
        if (req->headers_has("Accept-Encoding")
            && req->get_header("Accept-Encoding").find("gzip") != string::npos)
            str += "gzip";
        std::string hash = get_hash(str);
        //find req in data
        std::map<string, Response *>::const_iterator it = data.find(hash);
        if (it != data.end()) {
            //check stat info
            if (it->second->get_header("Last-Modified") == get_last_modif(it->second->path))
            {
                //update Date in resp
                it->second->set_header("Date", get_cur_date());
                return it->second;
            }
            delete it->second;
            data.erase(it);
        }
        return nullptr;
    }

    private:
    std::map<string, Response*> data;
};