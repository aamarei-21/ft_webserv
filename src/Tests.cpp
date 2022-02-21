#include "Tests.hpp"

void Tests::test_request() {
    string request = "GET /favicon.ico HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n";
    request += "Accept: */*\r\nConnection: keep-alive\r\n";
    request += "Cookie: csrftoken=1ee5mEoaHBnuz1675sgCQJUgdsKF\r\n";
    request += "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)\r\n";
    request += "Accept-Language: ru\r\nReferer: http://127.0.0.1:8080/\r\n";
    request += "Accept-Language: ru, en\r\nAccept-Encoding: gzip, deflate\r\n\r\n";

    Request req(worker.accept_request_headers);
    try { req.parse(request);}
    catch (const std::invalid_argument &ex) {
        std::cout << "Test Request fail - Error: " << ex.what() << '\n';
    }

    string request2 = "GET /favicon.ico HTTP/1.1\r\nHost: 192.168.0.1:8888\r\n";
    request2 += "Accept: */*\r\nConnection: keep-alive\r\n";
    request2 += "Cookie: csrftoken=1ee5mEoaHBnuz1675sgCQJUgdsKF\r\n";
    request2 += "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)\r\n";
    request2 += "Accept-Language: ru,en\r\nReferer: http://127.0.0.1:8080/\r\n";
    request2 += "Accept-Encoding: gzip, deflate\r\n\r\n";

    req.add_header("host", "192.168.0.1:8888");
    if (request2 == req.to_str())
        std::cout << "Test Request: OK" << std::endl;
    else
        std::cout << "Test Request: KO" << std::endl;
}

void Tests::test_cgj()
{
    char **envp = worker.get_env();
    int size = worker.ENV.size();
    for (char **env = envp; *env != 0; env++)
        ++size;
    char **new_env = new char*[size + 2];
    int i = 0;
    for ( ; i < size; ++i)
    {
        int len = std::strlen(envp[i]);
        new_env[i] = new char[len + 1];
        std::strcpy(new_env[i], envp[i]);  
    }
    std::string  strRequestBody = "from PyQt5.QtCore import Qt\n";
    strRequestBody += "from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QVBoxLayout\n";
    strRequestBody += "from random import randint\n\n";

    strRequestBody += "def show_winner():\n";
    strRequestBody += "    number = randint(0, 99)\n";
    strRequestBody += "    winner.setText(str(number))\n";
    strRequestBody += "    text.setText('Победитель:')\n\n";

    strRequestBody += "button.clicked.connect(show_winner)\n\n";

    strRequestBody += "main_win.show()\n";
    strRequestBody += "app.exec_()";
    const std::string strRequestHeader = "Content_Length=" + ToString((long long)strRequestBody.length());
    new_env[i] = new char[strRequestHeader.length() + 1];
    std::strcpy(new_env[i], strRequestHeader.c_str());
    new_env[++i] = nullptr;

    char **pszChildProcessArgs = new char*[3];
    pszChildProcessArgs[0] = new char[7];
    std::strcpy(pszChildProcessArgs[0], "/usr/bin/python3");
    pszChildProcessArgs[1] = new char[40];
    std::strcpy(pszChildProcessArgs[1], "./www/root/localhost/cgi-bin/testcgi.py");
    pszChildProcessArgs[2] = nullptr;

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
    if (req->headers_has("Cookie"))
        ENV["HTTP_COOKIE"] = req->get_header("Cookie");

    char **Env = get_env();
    char **Args = get_args(loc.getCGIPath(req->get_url()), resp->path);
    // std::cout << "CGIPath = " << Args[0] << "\n";
    // std::cout << "resp->path = " << Args[1] << "\n";
    // std::cout << "******************* body *****************\n";
    // std::cout << fix_length(1200, req->get_msg_body());
    // std::cout << "\n******************* end body *****************\n";

    CGIprocess proc;

    string result = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n  <title>Upload File</title>\n</head>\n<body>\n<p>";
    result += strRequestBody;
    result += "</p>\n<hr>\n<a href=\"/index.html\">Back to upload page</a>\n</body>\n</html>\n";
    try {
        proc.processing(strRequestBody, Args, Env);
    } catch (std::exception& e) {
        std::cerr << "CGI processing error: " << e.what() << '\n';
    }

    if (result == proc.response_body)
        std::cout << "Test cgi - OK" << std::endl;
    else
        std::cout << "Test cgi - KO" << std::endl;
    for (size_t k = 0; new_env[k] != nullptr; ++k)
        delete [] new_env[k];
    delete [] new_env;
    for (size_t k = 0; pszChildProcessArgs[k] != nullptr; ++k)
        delete [] pszChildProcessArgs[k];
    delete [] pszChildProcessArgs;
}

void Tests::test_cgj2()
{
    std::string tmp_env = read_file("/Users/macbook/Desktop/webserver/test/env_tester");
    std::vector <string> tmp1 = split(tmp_env, '\n');
    std::vector <string>::iterator it_tmp = tmp1.begin();
    char **new_env = new char*[tmp1.size() + 1];
    int i = 0;
    for (; it_tmp != tmp1.end(); ++it_tmp) {
        new_env[i] = new char[(*it_tmp).size() + 1];
        std::strcpy(new_env[i++], (*it_tmp).c_str());  
    }
    new_env[i] = nullptr;
    std::string  strRequestBody = read_file("/Users/macbook/Desktop/webserver/test/file_should_exist_after");

    char **pszChildProcessArgs = new char*[3];
    pszChildProcessArgs[0] = new char[7];
    std::strcpy(pszChildProcessArgs[0], "/Users/macbook/Desktop/webserver/www/root/localhost/cgi-bin/cgi_tester");
    pszChildProcessArgs[1] = new char[40];
    std::strcpy(pszChildProcessArgs[1], "/Users/macbook/Desktop/webserver/www/root/localhost/YoupiBanane/youpi.bla");
    pszChildProcessArgs[2] = nullptr;

    CGIprocess proc;

    string result = read_file("/Users/macbook/Desktop/webserver/test/result");
    proc.processing(strRequestBody, pszChildProcessArgs, new_env);
    std::cout << "******************* result CGI *****************\n";
    std::cout << fix_length(1000, proc.response_body) << "\n";
    std::cout << "\n******************* end result *****************\n";
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
            std::cout << "title - " << it_h->first << " value - " << it_h->second << "\n";
        }   
    }

    if (result == proc.response_body)
        std::cout << "Test cgi 2 - OK" << std::endl;
    else
        std::cout << "Test cgi 2 - KO" << std::endl;
    for (size_t k = 0; new_env[k] != nullptr; ++k)
        delete [] new_env[k];
    delete [] new_env;
    for (size_t k = 0; pszChildProcessArgs[k] != nullptr; ++k)
        delete [] pszChildProcessArgs[k];
    delete [] pszChildProcessArgs;
}

void Tests::test_cache() {
    string request1 = "GET / HTTP/1.1\r\nHost: 192.168.0.1:8888\r\n";
    request1 += "Accept-Encoding: gzip, deflate\r\n\r\n";

    string request2 = "GET / HTTP/1.1\r\nHost: 192.168.0.2:8888\r\n";
    request2 += "Accept-Encoding: gzip, deflate\r\n\r\n";

    string request3 = "GET /Users/macbook/Desktop/webserver/Makefile HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n";
    request3 += "Accept-Encoding: gzip, deflate\r\n\r\n";

    Request req1, req2, req3, req4;
    try { 
        req1.parse(request1);
        req2.parse(request2);
        req3.parse(request3);
        req4.parse(request3);
    } catch (const std::invalid_argument &ex) {
        std::cout << "Test Cache fail - Error: " << ex.what() << '\n';
    }

    Response *resp = new Response();
    resp->code = 200;
    resp->add_header("Allow", "GET, POST, DELETE");
    resp->add_header("Date", get_cur_date()); 
    resp->add_header("Last-Modified", get_last_modif("./Makefile")); 
    resp->path = "/Users/macbook/Desktop/webserver/Makefile";

    Cache cache;

    cache.add_response(&req1, resp);

    Response *resp2 = cache.get_response(&req2);
    if (!resp2) {
        resp2 = new Response(*resp);
        cache.add_response(&req2, resp2);
    }
    Response *resp3 = cache.get_response(&req3);
    if (!resp3) {
        resp3 = new Response(*resp);
        cache.add_response(&req3, resp3);
    }
    Response *resp4 = cache.get_response(&req4);
    if (!resp4) {
        resp4 = new Response(*resp);
        cache.add_response(&req4, resp4);
    }

    if (resp != resp2 && resp != resp3 && resp2 != resp3 && resp3 == resp4)
        std::cout << "Test Cache: OK" << std::endl;
    else
        std::cout << "Test Cache: KO" << std::endl;
}


void Tests::test_autoindex() {
    std::stringstream body;
    body << "<!DOCTYPE html>\n"
        << "<html lang=\"en\">\n"
        << "<head>\n"
        << "<meta charset=\"UTF-8\">\n"
	    << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        << "<meta name=\"robots\" content=\"none\"/>\n"
        << "<title>C++ HTTP WebServer Directory Listing: /</title>\n"
        << "</head>\n<body>\n";

    body << "<h1>Index of /</h1>\n<table>\n"
        << "<tr>\n<th>Type</th>\n<th>Name</th>\n<th>Size</th>\n"
        << "<th>Last Modified</th>\n</tr>\n\n";

    body << "<tr>\n<td>Dir</td>\n"
            << "<td><a href=\"/cgi-bin/\">cgi-bin/</a></td>\n<td></td>\n"
            << "<td></td>\n</tr>\n\n";

    body << "<tr>\n<td>Dir</td>\n"
            << "<td><a href=\"/uploads/\">uploads/</a></td>\n<td></td>\n"
            << "<td></td>\n</tr>\n\n";
    
    body << "<tr>\n<td>Dir</td>\n"
            << "<td><a href=\"/YoupiBanane/\">YoupiBanane/</a></td>\n<td></td>\n"
            << "<td></td>\n</tr>\n\n";
    
    body << "<tr>\n<td>File</td>\n"
            << "<td><a href=\"/favicon.ico\">favicon.ico</a></td>\n"
            << "<td>1150</td>\n"
            << "<td>Mon, 27 Dec 2021 08:10:45 GMT</td>\n</tr>\n\n";
    
    body << "<tr>\n<td>File</td>\n"
            << "<td><a href=\"/index.html\">index.html</a></td>\n"
            << "<td>1043</td>\n"
            << "<td>Tue, 28 Dec 2021 13:54:34 GMT</td>\n</tr>\n\n";
    
    body << "<tr>\n<td>File</td>\n"
            << "<td><a href=\"/robots.txt\">robots.txt</a></td>\n"
            << "<td>25</td>\n"
            << "<td>Mon, 27 Dec 2021 08:10:45 GMT</td>\n</tr>\n\n";
    
    body << "<tr>\n<td>File</td>\n"
            << "<td><a href=\"/start_cgi.sh\">start_cgi.sh</a></td>\n"
            << "<td>33</td>\n"
            << "<td>Mon, 27 Dec 2021 08:10:45 GMT</td>\n</tr>\n\n";
    
    body << "</table>\n";
    body << "<em><small>C++ Http WebServer</small></em>\n"
         << "</body>\n</html>";

    // std::cout << body.str() << std::endl;
    // std::cout << autoindex("/Users/macbook/Desktop/webserver/www/root/localhost", "/") << "\n";
    if (body.str() == autoindex("/Users/macbook/Desktop/webserver/www/root/localhost", "/"))
        std::cout << "Test Autoindex: OK" << std::endl;
    else
        std::cout << "Test Autoindex: KO" << std::endl;
}

void Tests::test_getPair() {
    string test = "/dir/main/exec";
    string test2 = "/dir/main/exec?dim=123&sim=545";
    string test3 = "/dir/main/exec?adr=mpscow%20lenin%20street";
    std::pair<string, string> ur = get_pair(test, '?');
    std::pair<string, string> ur2 = get_pair(test2, '?');
    std::pair<string, string> ur3 = get_pair(test3, '?');
    if ((ur.first != test || ur.second != "") ||
        (ur2.first != test || ur2.second != "dim=123&sim=545") ||
        (ur3.first != test || ur3.second != "adr=mpscow%20lenin%20street"))
        std::cout << "Test getPair: KO" << std::endl;
    else
        std::cout << "Test getPair: OK" << std::endl;
}