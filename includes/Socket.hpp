#pragma once

#include <iostream>
#include <string>
#include "Request.hpp"

using std::string;

class Socket {
public:
    u_int64_t write_buf_seek;
    u_int64_t write_buf_len;
    Socket(const string &type, const int &fd): _sockfd(fd), _type(type), ip_adr(),
                                            read_buf(""), write_buf(""),
                                            session_id(), csrftoken(), req(nullptr),
                                            keep_alive(true) {
        write_buf_len = 0;
        write_buf_seek = 0;
    }
    ~Socket() {
        // if (req != nullptr)
        //     delete req;
    }
    int get_fd() const { return _sockfd; }
    string get_type() const { return _type; }
    string get_ip_adr() const { return ip_adr; }
    bool get_keep_alive() const { return keep_alive; }
    
    string & get_read_buf() { return read_buf;}

    void clear_read_buf() {
        read_buf.clear();
    }

    const string & get_write_buf() {
        return write_buf;
    }

    void clear_write_buf() {
        write_buf.clear();
        write_buf_len = 0;
        write_buf_seek = 0;
    }

    void append_read_buf(const char *str) {read_buf += str;}
    void append_write_buf(const string &str) {
        write_buf += str;
        write_buf_len = write_buf.size();
    }

    bool read_buf_empty() const { return read_buf.empty(); }

    bool operator==(const Socket &oth) const {
        return this->_sockfd == oth.get_fd();
    }

    void set_ip_adr(string &adr) {ip_adr = adr;}

    void set_session(const string &hash) {session_id = hash;}
    bool check_session(const string &hash) const { return session_id == hash;}

    void set_csrftoken(const string &hash) {csrftoken = hash;}
    void set_keep_alive(bool flag) { keep_alive = flag;}
    bool check_csrftoken(const string &hash) const { return csrftoken == hash;}

    void set_req(Request *sep) { 
        req = sep;
    }
    Request* get_request() const { return req; }

private:
    int _sockfd;
    string _type;
    string ip_adr;
    string read_buf;
    string write_buf;
    string session_id;
    string csrftoken;
    Request *req;
    bool keep_alive;
//    u_int64_t body_length;
//    u_int64_t body_seek;
};