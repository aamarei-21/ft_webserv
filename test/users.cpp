#include <iostream>
#include "./includes/MD5.h"
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
#include <exception>
#include <stdexcept>
#include "./includes/utils.hpp"

using std::string;

string add_header(const string &title) {
    std::stringstream body;
    body << "<!DOCTYPE html>\n"
        << "<html lang=\"en\">\n"
        << "<head>\n"
        << "<meta charset=\"UTF-8\">\n"
	    << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        << "<meta name=\"robots\" content=\"none\"/>\n"
        << "<title>C++ HTTP WebServer " << title << "</title>\n"
        << "</head>\n<body>\n";
    return body.str();
}

std::string add_footer() {
    std::stringstream body;
    body << "<em><small>C++ Http WebServer</small></em>\n"
         << "</body>\n</html>";
    return body.str();
}

std::string gen_success_page(const string &login) {
    std::stringstream body;
        body << "<p>User '" << login << "' is autorizade!\n" 
        << "</p>\n"
        << "<a href=\"/\">Go back!</a>\n";

    return body.str();
}

class Sessions
{       
    public:
    std::string hash_anonymous;
    Sessions() {
        hash_anonymous = get_hash("anonymous_user passw0rd");
        if (!exists("./tmp_users_data.base")) {
            std::ofstream new_file("./tmp_users_data.base");
            new_file << hash_anonymous  << "#";
            new_file << "anonymous_user" << ";" << "passw0rd" << "\n";
            new_file.close();
        }
    }
    ~Sessions() {
    }

    // return session_id
    std::string add_new_user(const std::string &login,
        const std::string &password) {
        std::string hash = get_hash(login + " " + password);

        std::ifstream ibase_users("./tmp_users_data.base");
        for (std::string buf; std::getline(ibase_users, buf, '\n'); ) {
            if (buf.find(login) != std::string::npos) {
                std::pair <std::string, std::string> user_line = get_pair(buf, '#');
                std::pair <std::string, std::string> user_info = get_pair(user_line.second, ';');
                if (login == user_info.first) {
                    ibase_users.close();
                    return "";
                }
            }
        }

        std::ofstream obase_users("./tmp_users_data.base", std::ios_base::app);
        obase_users << hash << "#";
        obase_users << login << ";" << password << "\n";
        obase_users.close();
        return hash;
    }

    std::string get_user(const std::string &hash) {
        std::ifstream base_users("./tmp_users_data.base");

        for (std::string buf; std::getline(base_users, buf, '\n'); ) {
            if (!hash.empty() && buf.find(hash) != std::string::npos) {
                std::pair <std::string, std::string> user_line = get_pair(buf, '#');
                std::pair <std::string, std::string> user_info = get_pair(user_line.second, ';');
                return user_info.first;
            }
        }

        throw std::runtime_error("User not find");
    }

};

int main(int ac, char** av, char** env) {

    nonblock(0);
    nonblock(1);

    unsigned long buf_size = 65536;

    std::string response;
    Sessions sessions;

    (void)ac;
    (void)av;

    // resp->set_cookie("sessid=" + sessions.get_sessid() + "; HttpOnly");

    std::map<string, string> ENV;
    int i = 0;
    while (env[i]) {
        string str(env[i++]);
        std::pair <string, string> key_value = get_pair(str, '=');
        ENV[key_value.first] = key_value.second;
    }

    std::string login;
    std::string password;
    std::string session_id;
    std::string cookie_sessid="";

    try {
        std::vector <string> cookies = split(ENV.at("HTTP_COOKIE"), "; ");
        std::vector <string>::iterator it_cookie = cookies.begin();
        for ( ; it_cookie != cookies.end(); ++it_cookie) {
            if ((*it_cookie).empty() || (*it_cookie).find("=") == string::npos)
                continue;
            std::pair <string, string> cookie = get_pair(*it_cookie, '=');
            if (cookie.first == "sessid") {
                cookie_sessid = cookie.second;
                break;
            }
        }
    } catch (const std::exception &ex) {
        response += "Error_HTTP_COOKIE: " + string(ex.what()) + "\r\n";
    }

    std::string method = ENV["REQUEST_METHOD"];

    std::string body;

    response += "Content-Type: text/html; charset=UTF-8\r\n";

    if (method == "POST") {
        char bufferOut[buf_size + 1];
        int n = 1;
        while (n > 0) {
            n = read(0, bufferOut, buf_size);
            if (n > 0) {
                bufferOut[n] = '\0';
                body += std::string(bufferOut);
            }
        }
        if (!body.empty() && body.find("&") != string::npos) {
            std::vector <string> params = split(body, "&");
            std::vector <string>::iterator it = params.begin();
            for ( ; it != params.end(); ++it) {
                if ((*it).empty() || (*it).find("=") == string::npos)
                    continue;
                std::pair <string, string> pair = get_pair(*it, '=');
                if (pair.first == "login")
                    login = pair.second;
                if (pair.first == "password")
                    password = pair.second;
            }
            session_id = get_hash(login + " " + password);
            try {
                sessions.get_user(session_id);    
            } catch (const std::exception &) {
                session_id = sessions.add_new_user(login, password);
            }
        }
    } else {
        if (!cookie_sessid.empty()) {
            try {
                login = sessions.get_user(cookie_sessid);
                session_id = cookie_sessid;
            } catch (const std::exception &) {
                login = "anonymous_user";
                session_id = sessions.hash_anonymous;
            }
        }
    }
    if (login.empty() || session_id.empty()) {
        login = "anonymous_user";
        session_id = sessions.hash_anonymous;
    }
    if (cookie_sessid != session_id)
        response += "Set-Cookie: sessid=" + session_id + "; HttpOnly\r\n";
    response += "\r\n";
    response += add_header("Test users");
    response += gen_success_page(login);
    response += add_footer();

    // std::cout << response;

    unsigned long begin = 0;
    int chank_size = buf_size;
    // int global_size = 0;
    // std::cout << "start write-read\n";
    while (1) {
        if (begin < (unsigned long)response.size()) {
            //Читаем ответ от дочернего процесса
            if (response.size() - begin < buf_size)
                chank_size = response.size() - begin;
            int k = write(1, response.c_str() + begin, chank_size);
            if (k >= 0) {
                // std::cout << "write " << k << " byte\n";
                begin += k;
            }
        } else {
            return 0;
        }
    }
    return 0;
}

// clang++ users.cpp ./src/MD5.cpp ./src/utils.cpp -I./includes -o users.cgi && cp users.cgi ./www/root/localhost/cgi-bin/
