#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include "Socket.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "CGIprocess.hpp"
#include "Cache.hpp"
#include "Location.hpp"
#include "utils.hpp"
#include <exception>
#include "File.hpp"
#include "pages.hpp"

using std::string;

typedef std::vector<Server>::iterator serv_iter;

//todo заменить Вектор ЕНВ на Мап ?
class Worker {
private:
    Config &conf;
    Cache cache;
    File file_types;
    Request *req;
    Response *resp;
    
public:
    std::map<string, string> ENV;
    string accept_request_headers;
    string accept_response_headers;
    std::map<int, string> status_info;

    Worker(Config &config, char **env): conf(config), cache(), file_types(),
                                        req(nullptr), resp(nullptr), ENV(), accept_request_headers(),
                                        accept_response_headers(), status_info()  {
        int i = 0;
        while (env[i]) {
            string str(env[i++]);
            std::pair <string, string> key_value = get_pair(str, '=');
            ENV[key_value.first] = key_value.second;
        }
        accept_request_headers = "Referer User-Agent Content-Language Cookie Accept ";
        accept_request_headers += "Accept-Charset Accept-Encoding Accept-Language Authorization ";
        accept_request_headers += "Cache-Control Connection Content-Disposition Content-Encoding ";
        accept_request_headers += "Content-Length Content-Location Content-MD5 Content-Range ";
        accept_request_headers += "Content-Type Content-Version Date Derived-From Expect Expires ";
        accept_request_headers += "From Host If-Match If-Modified-Since If-None-Match If-Range ";
        accept_request_headers += "If-Unmodified-Since Last-Modified Link Max-Forwards MIME-Version ";
        accept_request_headers += "Pragma Proxy-Authorization Range Title TE Trailer Transfer-Encoding ";
        accept_request_headers += "sec-ch-ua-mobile sec-ch-ua-platform Upgrade-Insecure-Requests ";
        accept_request_headers += "sec-ch-ua Sec-Fetch-Mode Sec-Fetch-User Sec-Fetch-Dest Sec-Fetch-Site ";
        accept_request_headers += "Upgrade Via Warning X-Secret-Header-For-Test ";
        accept_response_headers = "Content-Language Cookie Age Accept-Ranges Allow Alternates ";
        accept_response_headers += "Cache-Control Connection Content-Disposition Content-Encoding ";
        accept_response_headers += "Content-Length Content-Location Content-MD5 Content-Range ";
        accept_response_headers += "Content-Type Content-Version Date Derived-From ETag Expires ";
        accept_response_headers += "Last-Modified Link Location MIME-Version Retry-After Server ";
        accept_response_headers += "Pragma Proxy-Authenticate Public Title Trailer Transfer-Encoding ";
        accept_response_headers += "Upgrade Vary Via Warning WWW-Authenticate ";
        status_info[100] = "Continue";
        status_info[101] = "Switching Protocols";
        status_info[102] = "Early Hints";
        status_info[200] = "OK";
        status_info[201] = "Created";
        status_info[202] = "Accepted";
        status_info[203] = "Non-Authoritative Information";
        status_info[204] = "No Content";
        status_info[205] = "Reset Content";
        status_info[206] = "Partial Content";
        status_info[300] = "Multiple Choices";
        status_info[301] = "Moved Permanently";
        status_info[302] = "Found";
        status_info[303] = "See Other";
        status_info[304] = "Not Modified";
        status_info[307] = "Temporary Redirect";
        status_info[308] = "Permanent Redirect";
        status_info[400] = "Bad Request";
        status_info[401] = "Unauthorized";
        status_info[402] = "Payment Required";
        status_info[403] = "Forbidden";
        status_info[404] = "Not Found";
        status_info[405] = "Method Not Allowed";
        status_info[406] = "Not Acceptable";
        status_info[407] = "Proxy Authentication Required";
        status_info[408] = "Request Timeout";
        status_info[409] = "Conflict";
        status_info[410] = "Gone";
        status_info[411] = "Length Required";
        status_info[412] = "Precondition Failed";
        status_info[413] = "Payload Too Large";
        status_info[414] = "URI Too Long";
        status_info[415] = "Unsupported Media Type";
        status_info[416] = "Range Not Satisfiable";
        status_info[417] = "Expectation Failed";
        status_info[418] = "I'm a teapot";
        status_info[429] = "Too Many Requests";
        status_info[431] = "Request Header Fields Too Large";
        status_info[500] = "Internal Server Error";
        status_info[501] = "Not Implemented";
        status_info[502] = "Bad Gateway";
        status_info[503] = "Service Unavailable";
        status_info[504] = "Gateway Timeout";
        status_info[505] = "HTTP Version Not Supported";
        status_info[506] = "Variant Also Negotiates";
        status_info[507] = "Insufficient Storage";
        status_info[508] = "Loop Detected";
        status_info[510] = "Not Extended";
        status_info[511] = "Network Authentication Required";
    }

    int parsing(Socket &sock) {
        string buf = sock.get_read_buf();
        req = sock.get_request();
        if (!req) {
            if (buf.empty() || buf == "\r\n" || buf == "\r" || buf == "\n") {
                sock.clear_read_buf();
                return 0;
            }
            req = new Request(accept_request_headers);
        }
        // std::cout << "\nread buffer: \n" << fix_length(1000, buf) << "\n\n";
        // std::cout << "read buffer size : " << buf.size() << "\n";
        // std::cout << '#';
        resp = new Response(status_info, accept_response_headers);
        resp->set_header("Date", get_cur_date());
        try {
            req->parse(buf);
        } catch (const std::invalid_argument &ex) {
            std::cerr << "Error: " << ex.what() << "\nread buffer:\n";
            std::cerr << fix_length(1000, buf) << '\n';
            resp->code = 501;
            processing_errors(resp, conf.servers.end(), nullptr);
            string response = resp->to_str();
            resp->print_response();
            // print_response(response);
            sock.append_write_buf(response);
            delete req;
            delete resp;
            sock.set_req(nullptr);
            sock.clear_read_buf();
            return 0;
        }
        delete resp;
        if (!req->get_end()) {
            sock.set_req(req);
            return 0;
        }
        print_request(req);
        return 1;
    }

    int find_in_cache(Socket &sock) {
        Response *resp2 = cache.get_response(req);
        if (resp2) {
            string response = resp2->to_str();
            delete req;
            sock.set_req(nullptr);
            resp2->print_response("from cache ");
//             print_response(response);
            sock.append_write_buf(response);
            return 1;
        }
        return 0;
    }

    void processing(Socket &sock) {
        if (req->get_header("Connection") == "close")
            sock.set_keep_alive(false);
        if (find_in_cache(sock))
            return;
        resp = new Response(status_info, accept_response_headers);
        resp->set_header("Date", get_cur_date());
        serv_iter it = get_serv(*req);
        if (it == conf.servers.end()) {
            resp->code = 500;
            processing_errors(resp, it, nullptr);
        } else {
            Location const &loc = it->getLocation(req->get_url());
            resp->path = it->getFile(req->get_url(), req->get_method_e(), resp->code, loc);
            if (resp->code >= 200 && resp->code <= 206) {
                if (loc.isCGI(resp->path, req->get_method_e()))
                    processing_CGI(sock, req, resp, loc);
                else {
                    switch (req->get_method_e()) {
                    case POST:
                        if (loc.getMaxBody() == 0 || req->get_msg_body().size() <= loc.getMaxBody())
                            processing_POST_end(req, resp, loc);
                        else
                            resp->code = 413;
                        break;
                    case PUT:
                        processing_PUT(req, resp, loc);
                        break;
                    case GET:
                        processing_GET(req, resp);
                        cache.add_response(req, resp);
                        break;
                    case DELETE:
                        processing_DELETE(resp);
                        break;  
                    default:
                        resp->code = 418;
                        break;
                    }
                }
            } else if (resp->code >= 300 && resp->code <= 308) {
                processing_Redirect(resp);
            }
            if (resp->code == 404 && req->get_method_e() == PUT)
                processing_PUT(req, resp, loc);
            if (resp->code >= 400)
                processing_errors(resp, it, &loc);
        }
        string response = resp->to_str();
        resp->print_response();
        if (cache.last_response != resp)
            delete resp;

        delete req;
        sock.set_req(nullptr);
//		print_response(response);
        sock.append_write_buf(response);
    }

    void processing_Redirect(Response *resp) {
        resp->set_header("Location", resp->path);
    }

    void processing_errors(Response *resp, serv_iter it, const Location *loc) {
        if (resp->code >= 500 && resp->code <= 511) {
            if (it != conf.servers.end()) {  // todo подумать что здесь лучше, нужно ли к 1 серверу отдать запрос
                string path = it->getErrorPagePath(resp->code);
                if (!path.empty())
                    resp->set_body(read_file(path));
            } else
                resp->set_body(gen_error_page(ToString(resp->code), resp->get_status()));
        }
        else if (it != conf.servers.end() && resp->code >= 400 && resp->code <= 418) {
            string path = it->getErrorPagePath(resp->code);
            if (!path.empty())
                resp->set_body(read_file(path));
            else
                resp->set_body(gen_error_page(ToString(resp->code), resp->get_status()));
            if (resp->code == 405 && loc)
                resp->add_header("Allow", loc->getAllow());
        }
        resp->add_header("Content-Type", "text/html; charset=utf-8");
        resp->add_header("Content-Length", (long long int)resp->get_msg_body().length());
    }

    void processing_GET(Request *req, Response *resp) {
        if (resp->path.back() == '/') {
            resp->set_body(autoindex(resp->path, req->get_url()));
            resp->add_header("Cache-Control", "no-cache");
            resp->add_header("Content-Type", "text/html");
            // resp->add_header("Expires", get_expires_date(1));
        } else {
            //todo добавить обработку не строковых файлов через vector char
            resp->set_body(read_file(resp->path));
            resp->add_header("Content-Type", file_types.get_file_type(resp->path));
            resp->add_header("Last-Modified", get_last_modif(resp->path));
        }
        if (req->get_header("Connection") == "keep-alive")
            resp->add_header("Connection", "keep-alive");
        compressed_response(req, resp, resp->get_msg_body());
        resp->set_header("Content-Length", ToString(resp->get_msg_body().length()));
    }

    void compressed_response(Request *req, Response *resp, const string &body) {
        if (req->headers_has("Accept-Encoding")) {
            if (req->get_header("Accept-Encoding").find("gzip") != string::npos
                    && body.length() > 10000) {
                std::string path = "/Volumes/RAMDisk/file";
                std::ofstream upfile(path, std::ios_base::binary);
                upfile << body;
                upfile.close();
                std::string command = "gzip " + path;
                std::system(command.c_str());
                path += ".gz";
                // // read tmp file
                resp->set_body(read_file(path));
                command  = "rm -rf " + path;
                std::system(command.c_str());
                resp->set_header("Content-Encoding", "gzip");
            }
        }
    }

    serv_iter get_serv(Request &req) {
        std::vector<serv_iter> found_servs;
        serv_iter it = conf.servers.begin();
        for ( ; it != conf.servers.end(); ++it) {
            // std::cout << it->getAddressStr() + ":" + ToString(it->getPort()) << std::endl;
            if (str_cmp(req["Host"], it->getAddressStr() + ":" + ToString(it->getPort()))) 
                found_servs.push_back(it);
        }
        if (found_servs.size() == 1)
            return found_servs.back();
        it = (found_servs.size() == 0)? conf.servers.begin(): found_servs.at(0);
        serv_iter end = (found_servs.size() == 0)? conf.servers.end(): found_servs.back();
        for ( ; it != end; ++it) {
            for (size_t i = 0; i < it->getServerNames().size(); ++i) {
                if (str_cmp(req["Host"], it->getServerNames()[i] + ":" + ToString(it->getPort())))
                    return it;
            }
        }
        if (found_servs.size() > 0)
            return found_servs.at(0);
        return conf.servers.end();
    }

    void processing_DELETE(Response *resp) {
        if (std::remove(resp->path.c_str()) == 0)
            resp->code = 204;
        else
            resp->code = 403;
    }

    void processing_POST_end(Request *req, Response *resp, const Location &loc) {
        (void)resp;
        if (!req->sep.empty()) {
            std::vector <string> part = split(req->get_msg_body(), ("--" + req->sep));
            std::vector <string>::iterator it_part = part.begin();
            for ( ; it_part != part.end(); ++it_part) {
                if ((*it_part).empty())
                    continue;
                std::vector <string> tmp = split(*it_part, '\n');
                std::vector <string>::iterator it = tmp.begin();
                if (it == tmp.end())
                    continue;
                // Headers
                std::map<string, string, Comparator> headers;
                string last_header = "";
                int flag = 0;
                for (++it; it != tmp.end() && !(reduce(*it).empty()); ++it) {
                    if (flag && it->at(0) == ' ')
                        headers[last_header] += reduce(*it);
                    else if (it->at(0) == ' ')
                        continue;
                    else {
                        std::pair <string, string> header = get_pair(*it, ':');
                        flag = 0;
                        if (!add_to_header(headers, header.first, header.second, last_header))
                            continue;
                        flag = 1;
                    }
                }
                while (it != tmp.end() && reduce(*it).empty())
                    ++it;
                // обработка тела выделенной части
                std::vector <string> keys = split(headers["Content-Disposition"], ';');
                std::vector <string>::iterator it_key = keys.begin();
                for (; it_key != keys.end(); ++it_key) {
                    std::pair <string, string> key_pair = get_pair(*it_key, '=');
                    if (reduce(key_pair.first) == "filename") {
                        string abs_path = loc.getUploadPath(req->get_url()) + "/" + trim(key_pair.second, "/'\"");
                        resp->code = (exists(abs_path))? 200: 201;
                        std::ofstream upfile(abs_path, std::ios_base::binary);
                        for (; it != tmp.end() && !((reduce(*it) == ("--" + req->sep) || reduce(*it).empty()) && it + 1 == tmp.end()); ) {
                            upfile << *(it++);
                            if (it != tmp.end() && !((reduce(*it) == ("--" + req->sep) || reduce(*it).empty()) && it + 1 == tmp.end()))
                                upfile << "\n";
                        }
                        upfile.close();
                    }
                }
            }
        }
        resp->set_body(gen_success_page(ToString(resp->code)));
        resp->add_header("Content-Type", "text/html; charset=UTF-8");
        resp->set_header("Content-Length", ToString(resp->get_msg_body().length()));
    }

    void processing_PUT(Request *req, Response *resp, const Location &loc) {
        string abs_path = loc.getUploadPath(req->get_url());
        if (!abs_path.empty()) {
            resp->set_header("Content-Location", req->get_url());
            resp->code = (exists(abs_path))? 200: 201;
            std::ofstream upfile(abs_path, std::ios_base::binary);
            upfile << req->get_msg_body();
            upfile.close();
        }
        resp->set_header("Connection", "close");
        
        resp->set_body(gen_success_page(ToString(resp->code)));
        resp->add_header("Content-Type", "text/html; charset=UTF-8");
        resp->add_header("Content-Length", (long long int)resp->get_msg_body().length());
    }

    void processing_CGI(Socket &sock, Request *req, Response *resp, const Location &loc) {
        // std::cout << "CGI start\n";
        if (req->get_method_e() == POST) {
            ENV["REQUEST_METHOD"] = "POST";
            if (req->headers_has("Content-Type"))
                ENV["CONTENT_TYPE"] = req->get_header("Content-Type");
            ENV["CONTENT_LENGTH"] = ToString(req->get_msg_body().size());
        } else if (req->get_method_e() == GET) {
            ENV["REQUEST_METHOD"] = "GET";
        } else
            ENV["REQUEST_METHOD"] = "other";
        ENV["REMOTE_ADDR"] = sock.get_ip_adr();
        ENV["QUERY_STRING"] = req->get_params();
        if (req->headers_has("User-Agent"))
            ENV["HTTP_USER_AGENT"] = req->get_header("User-Agent");
        if (req->headers_has("X-Secret-Header-For-Test"))
            ENV["HTTP_X_SECRET_HEADER_FOR_TEST"] = req->get_header("X-Secret-Header-For-Test");
        ENV["SERVER_NAME"] = req->get_header("Host");
        ENV["SERVER_PROTOCOL"] = req->get_version();
        ENV["UPLOAD_DIR"] = loc.getUploadPath("");
        ENV["PATH_INFO"] = resp->path;
        if (req->headers_has("Cookie")) {
			ENV["HTTP_COOKIE"] = req->get_header("Cookie");
		}

        CGIprocess proc;

        char **Env = get_env();
        char **Args = get_args(loc.getCGIPath(req->get_url()), resp->path);
//         std::cout << "CGIPath = " << Args[0] << "\n";
//         std::cout << "resp->path = " << Args[1] << "\n";
//         std::cout << "******************* body *****************\n";
//         std::cout << fix_length(1200, req->get_msg_body());
//         std::cout << "\n******************* end body *****************\n";
        try {
            proc.processing(req->get_msg_body(), Args, Env);
//            std::cout << "******************* result CGI *****************\n";
//            std::cout << fix_length(500, proc.response_body) << " len " << proc.response_body.size() << "\n";
//            std::cout << "\n******************* end result *****************\n";
            std::vector <string> tmp = split(proc.response_body, "\r\n");
            std::vector <string>::iterator it = tmp.begin();
            if (it != tmp.end()) {
                // Headers
                std::map<string, string, Comparator> headers;
                string last_header = "";
                int flag = 0;
                for (; it != tmp.end() && !reduce(*it, " \t\r\n").empty(); ++it) {
                    if (flag && it->at(0) == ' ')
                        headers[last_header] += reduce(*it);
                    else if (it->at(0) == ' ')
                        continue;
                    else {
                        std::pair <string, string> header = get_pair(*it, ':');
                        flag = 0;
                        if (!add_to_header(headers, header.first, header.second, last_header))
                            continue;
                        flag = 1;
                    }
                }
                std::map<string, string>::iterator it_h = headers.begin();
                for (; it_h != headers.end(); ++it_h) {
                    // std::cout << "title - " << it_h->first << " value - " << it_h->second << "\n";
                    resp->set_header(it_h->first, it_h->second);
                }
                while (it != tmp.end() && reduce(*it, " \t\r\n").empty()) {
                    ++it;
                }
                if (it != tmp.end()) {
                    if (req->headers_has("Accept-Encoding")
                            && req->get_header("Accept-Encoding").find("gzip") != string::npos
                            && (*it).length() > 10000)
                        compressed_response(req, resp, *it);
                    else
                        resp->set_body(*it);
                }
            }
        } catch (std::exception& e) {
            std::cerr << "CGI processing error: " << e.what() << '\n';
        }
        
        free_array(Env);
        free_array(Args);
        clear_ENV();
        resp->set_header("Content-Length", ToString(resp->get_msg_body().length()));
        // std::cout << "CGI finish\n";
    }

    void clear_ENV() {
        ENV.erase("REQUEST_METHOD");
        ENV.erase("CONTENT_TYPE"); 
        ENV.erase("CONTENT_LENGTH"); 
        ENV.erase("REMOTE_ADDR"); 
        ENV.erase("QUERY_STRING"); 
        ENV.erase("HTTP_USER_AGENT"); 
        ENV.erase("HTTP_X_SECRET_HEADER_FOR_TEST"); 
        ENV.erase("SERVER_NAME"); 
        ENV.erase("SERVER_PROTOCOL");
        ENV.erase("UPLOAD_DIR");
        ENV.erase("PATH_INFO");
        ENV.erase("HTTP_COOKIE");
    }

    char** get_args(const string &filename, const string &args) {
        char** cstrings = new char*[3];
        string str = filename;
        if (!exists(str)) {
            std::vector<string> path = split(ENV["PATH"], ':');
            for(size_t i = 0; i < path.size(); ++i) {
                str = path[i] + "/" + filename;
                if (exists(str))
                    break;
            }
        }
        cstrings[0] = new char[str.length() + 1];
        std::strcpy(cstrings[0], str.c_str());
        if (!args.empty()) {
            cstrings[1] = new char[args.length() + 1];
            std::strcpy(cstrings[1], args.c_str());
        } else
            cstrings[1] = nullptr;
        cstrings[2] = nullptr;

        return cstrings;
    }

    char** get_env() {
        char** cstrings = new char*[ENV.size() + 1];

        size_t i = 0;
        std::map<string, string>::const_iterator it = ENV.begin();
        for( ; it != ENV.end(); ++it) {
            if (!it->second.empty()) {
                cstrings[i] = new char[it->first.length() + it->second.length() + 2];
                string str = it->first + "=" + it->second;
                // std::cout << str << std::endl;
                std::strcpy(cstrings[i], str.c_str());
                ++i;
            }
        }
        cstrings[i] = nullptr;

        return cstrings;
    }

    

    void print_request(Request *req) {
        req->print_request();
//         std::cout << "\n****************** REQUEST ******************\n";
//         std::cout << fix_length(800, req->to_str()) << std::endl;
//         std::cout << "*********************************************\n";
    }

    void print_response(const string &response) {
        std::cout << "\n****************** RESPONSE ******************\n";
        std::cout << fix_length(800, response) << std::endl;
        std::cout << "**********************************************\n";
    }
};