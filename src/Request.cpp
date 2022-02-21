#include "Request.hpp"

void Request::parse(string &raw) {
    this->end = false;
    if (headers_has("Transfer-Encoding") && "chunked" == get_header("Transfer-Encoding")) {
        if (raw.substr(raw.size() - 7) != "\r\n0\r\n\r\n") {
            // std::cout << "#";
            return;
        }
    }
    std::vector <string> tmp = split(raw, '\n');
    std::vector <string>::iterator it = tmp.begin();
    this->size_chanks = 0;
    this->indexes.clear();
    this->tokens.clear();
    this->msg_body.clear();
    while (it != tmp.end() && reduce(*it).empty())
        ++it;
    if (it == tmp.end())
        return;
    // Method Uri Version
    // std::cout << "Start line - " << reduce(*it) << "\n";
    std::vector <string> start_line = split(reduce(*it), ' ');
    if (start_line.size() < 2)
        return;
    this->method = "";
    this->method_e = 0;
    this->set_method(start_line[0]);
    this->uri = start_line[1];
    std::pair<string, string> ur = get_pair(this->uri, '?');
    this->url = ur.first;
    this->params = ur.second;
    if (start_line.size() > 2)
        this->version = start_line[2];
    read_line = false;
    ++it;
    // Headers
    uint32_t index = 0;
    int flag = 0;
    for ( ; it != tmp.end() && !(reduce(*it).empty()); ++it) {
        if (flag && it->at(0) == ' ')
            this->tokens[index].second += reduce(*it);
        else if (it->at(0) == ' ')
            continue;
        else {
            std::pair <string, string> header = get_pair(*it, ':');
            flag = 0;
            if (!add_to_header(header.first, header.second, index))
                continue;
            flag = 1;
        }
    }
    if (it == tmp.end()) {
        this->end = false;
        return;
    }
    int k = 0;
    while (it != tmp.end() && reduce(*it).empty()) {
        ++it;
        ++k;
    }
    this->end = (k > 0)? true: false;
    if (headers_has("Transfer-Encoding") 
                && (*this)["Transfer-Encoding"] == "chunked") {
        for (; it != tmp.end() && it + 1 != tmp.end() && 
                !(reduce(*(it)) == "0" && reduce(*(it+1)).empty()); it += 2) {
            // this->size_chanks += ToNumber<uint64_t>(reduce(*(it)), 16);
            this->msg_body += reduce(*(it+1));
            if (it + 2 == tmp.end() && raw.back() == '\n')
                this->msg_body += "\n";
        }
        // if (this->msg_body.length() != (size_t)this->size_chanks)
        //     std::cout << "\nError chanks size sum!!\n";
        if (it == tmp.end() || it + 1 == tmp.end())
            this->end = false;
        else if (reduce(*(it)) != "0" || !reduce(*(it+1)).empty())
            this->end = false;
    }
    else {
        for (; it != tmp.end(); ) {
            this->msg_body += *it;
            if (++it != tmp.end() || raw.back() == '\n')
                this->msg_body += "\n";
        }
        // Check Content-Type: multipart "Закрывающий" boundary separator
        if (!this->sep.empty() || (headers_has("Content-Type") &&
                (std::strncmp((*this)["Content-Type"].c_str(), "multipart/", 10) == 0))) {
            --it;
            if (this->sep.empty()) {
                std::pair <string, string> con_type = get_pair((*this)["Content-Type"], ';');
                std::pair <string, string> bound = get_pair(trim(con_type.second), '=');
                this->sep = trim(bound.second, "'\"");
            }
            if (reduce(*(it)) != ("--" + this->sep + "--"))
                this->end = false;
        }
    }
}

int Request::add_to_header(const string &first, const string &second, u_int32_t &index) {
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

string Request::to_str() const {
    string request = this->method + " " + this->uri + " " + this->version + "\r\n";
    for (u_int32_t i = 0; i < this->indexes.size(); ++i) {
        request += this->tokens[i].first + ": " + this->tokens[i].second + "\r\n";
    }
    request += "\r\n";
    request += this->msg_body;
    return request;
}

int Request::check_method(const string &m) {
    string methods = "GET HEAD POST PUT DELETE CONNECT OPTIONS TRACE PATCH";
    if (!m.empty() && methods.find(m) == string::npos)
        return 0;
    return 1;
}