#include "AHttpMsg.hpp"

std::map<string, string > AHttpMsg::get_headers() const {
    std::map<string, string> headers;

    std::copy(tokens.begin(), tokens.end(),
              std::inserter(headers, headers.begin()));
    return headers;
}

void AHttpMsg::set_header(const string &first, const string &second) {
    string key = reduce(first);
    std::map<string, u_int32_t>::const_iterator it = indexes.find(key);
    if (it != indexes.end())
        tokens[it->second].second = reduce(second);
    else {
        u_int32_t index = this->indexes.size();
        this->indexes[key] = index;
        this->tokens.push_back(std::make_pair(key, reduce(second)));
    }
}

int AHttpMsg::add_header(const string &first, const string &second) {
    u_int32_t index = 0;
    return add_to_header(first, second, index);
}

int AHttpMsg::add_header(const string &first, const long long int &second) {
    std::stringstream ss;
    ss << second;
    return add_header(first, ss.str());
}

int AHttpMsg::add_header(const string &first, const double &second) {
    std::stringstream ss;
    ss << second;
    return add_header(first, ss.str());
}

void AHttpMsg::add_value_in_header(u_int32_t index, const string &str) {
    string old_val = reduce(tokens[index].second, ",", " ,");
    if (count(old_val, ',') > 0 || count(str, ',') > 0) {
        std::vector<string> tmp = split(reduce(str, ",", " ,"), ',');
        for (u_int32_t i = 0; i < tmp.size(); ++i) {
            if (!tmp[i].empty() && old_val.find(tmp[i]) == string::npos)
                old_val += "," + tmp[i];
        }
        tokens[index].second = old_val;
    } else {
        tokens[index].second = str;
    }
}
