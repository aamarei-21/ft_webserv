#include "utils.hpp"

// Util for parse Http request and response
std::vector<string> split(const string& s, char delimiter) {
    std::vector<string> tokens;
    string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

std::vector<string> split(const string& s, const string &delimiter) {
    std::vector<string> tokens;
    string token;
    size_t pos_begin = 0;
    size_t pos_end = s.size();
    std::vector<char> data(s.begin(), s.end());
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == delimiter[0]) {
            bool flag = true;
            for (size_t k = 1; k < delimiter.size(); ++k) {
                if (data[i + k] != delimiter[k]) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                pos_end = i;
                token = std::string(data.begin() + pos_begin, data.begin() + pos_end);
                tokens.push_back(token);
                pos_begin = pos_end + delimiter.length();
            }
        }
    }
    token = std::string(data.begin() + pos_begin, data.end());
    tokens.push_back(token);
    return tokens;
}

string trim(const string& str, const string& whitespace) {
    const size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return std::string(); // no content

    const size_t strEnd = str.find_last_not_of(whitespace);
    const size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

string reduce(const string& str, const string& fill,
                   const string& whitespace){
    // trim first
    string result = trim(str, whitespace);

    // replace sub ranges
    size_t beginSpace = result.find_first_of(whitespace);
    while (beginSpace != string::npos) {
        const size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
        const size_t range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const size_t newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}

string urlEncode(const string &str) {
    string new_str = "";
    char c;
    int ic;
    const char* chars = str.c_str();
    char bufHex[10];
    int len = strlen(chars);

    for(int i=0;i<len;i++){
        c = chars[i];
        ic = c;
        // uncomment this if you want to encode spaces with +
        /*if (c==' ') new_str += '+';
        else */if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') new_str += c;
        else {
            sprintf(bufHex,"%X",c);
            if(ic < 16)
                new_str += "%0";
            else
                new_str += "%";
            new_str += bufHex;
        }
    }
    return new_str;
}

string urlDecode(const string &str) {
    string ret;
    char ch;
    int i, ii, len = str.length();

    for (i=0; i < len; i++){
        if(str[i] != '%'){
            if(str[i] == '+')
                ret += ' ';
            else
                ret += str[i];
        }else{
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return ret;
}

std::pair<string, string> get_pair(const string &s, char delimiter) {
    size_t pos = s.find_first_of(delimiter);
    string key = s.substr(0, pos);
    if (pos == string::npos)
        return std::make_pair(key, "");
    string value = s.substr(pos + 1);
    return std::make_pair(key, value);
}

u_int32_t count(const string &s, char f) {
    u_int32_t k = 0;
    for (u_int32_t i = 0; i < s.length(); ++i) {
        if (s[i] == f)
            ++k;
    }
    return k;
}

string find_lower(const string &header, const string &headers) {
    string str1 = ToLower(header);

    std::vector<string> list = split(headers, ' ');
    std::vector <string>::iterator it = list.begin();
    for ( ; it != list.end(); ++it) {
        string str2 = ToLower (*it);
        if (str1 == str2)
            return *it;
    }
    return std::string();
}

string ToLower ( const string &Text )
{
    string str1(Text.length(), ' ');
    std::transform(Text.begin(), Text.end(), str1.begin(), tolower);
    return str1;
}

bool str_cmp(const string &str1, const string &str2) {
    if (str1.length() != str2.length())
        return false;
    for (size_t i = 0; i < str1.length() && i < str2.length(); ++i) {
        if (str1[i] != str2[i])
            return false;
    }
    return true;
}

bool exists(const string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

string get_cur_date()
{
	char buffer[80];
	time_t seconds = time(NULL);
	tm* timeinfo = gmtime(&seconds);
	strftime(buffer, 80, "%a, %d %b %Y %I:%M:%S GMT", timeinfo);
	return std::string(buffer);
}

string get_expires_date(int min, int hours, int days) {
    char buffer[80];
	time_t seconds = time(NULL);
    seconds += min * 60 + hours * 3600 + days * 86400;
	tm* timeinfo = gmtime(&seconds);
	strftime(buffer, 80, "%a, %d %b %Y %I:%M:%S GMT", timeinfo);
	return std::string(buffer);
}

string get_last_modif(const string &filename)
{
	int ret;
	struct stat buf;
	if ((ret = stat(filename.c_str(), &buf)) != 0) {
		return std::string();
	}
    string old_val = ToString(asctime(gmtime(&buf.st_mtime)));
    std::vector<string> tmp = split(old_val, ' ');
    std::stringstream new_val;
    new_val << tmp[0] << ", " << tmp[2] << " " << tmp[1] << " " 
        << trim(tmp[4], "\n") << " " << tmp[3] << " GMT";
	return new_val.str();
}

string get_file_size(const string &filename)
{
	int ret;
	struct stat buf;
	if ((ret = stat(filename.c_str(), &buf))!=0) {
		std::cerr << "stat failure error ." << ret << std::endl;
	}
	return ToString(buf.st_size);
}

string read_file(const string &path) {
    // std::cout << "read file " << path << "\n";
    std::ifstream ifs(path, std::ios_base::binary);
    if (!ifs.is_open())
        return std::string();
    
    string data = string(std::istreambuf_iterator<char>(ifs),
                                    std::istreambuf_iterator<char>());
    ifs.close();
    return data;
    //std::vector<char> data = std::vector<char>(std::istreambuf_iterator<char>(ifs),
    //                                            std::istreambuf_iterator<char>());
    //std::for_each(data.begin(), data.end(), [](char c) { std::cout << c; });
}

string get_hash(const string &s1)
{
    MD5 md5(s1);
    md5.finalize();
    return md5.hexdigest();

    // unsigned long long hash = 5381;
    // int c;
    // size_t end = s1.length();
    // for (size_t i = 0; i < end; ++i) {
    //     c = s1[i];
    //     hash = ((hash << 5) + hash) + c; // hash * 33 + c 
    // }
    // return ToString(hash);
}

string fix_length(size_t len, const string &str) {
    string tmp;

	if (str.length() > len) {
        tmp.append(str, 0, len);
        tmp += "\n....\n";
        return tmp;
    }
    return str;
}

void free_array(char **arr) {
    size_t i = 0;
    while (arr[i] != nullptr) {
        delete [] arr[i++];
    }
    delete [] arr;
}

void nonblock(int sockfd)
{
    int opts;
    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) {
        std::cerr << "fcntl(F_GETFL)\n";
        exit(1);
    }
    opts = (opts | O_NONBLOCK);
    if(fcntl(sockfd, F_SETFL, opts) < 0) {
        std::cerr << "fcntl(F_SETFL)\n";
        exit(1);
    }
}

int add_to_header(std::map<string, string, Comparator> &headers, const string &first, const string &second, string &last_header) {
    string headers_str = "Content-Disposition Content-Encoding Set-Cookie ";
    headers_str += "Content-Length Content-Location Content-MD5 Content-Range ";
    headers_str += "Content-Type Content-Version Date Status";
    string key;
    if (headers_str.find(first) != string::npos)
        key = first;
    else {
        key = find_lower(first, headers_str);
        if (key.empty())
            return 0;
    }

    std::map<string, string>::const_iterator it = headers.find(key);
    if (it != headers.end())
        add_value_in_header(key, headers, reduce(second));
    else {
        last_header = key;
        headers[key] = reduce(second);
    }
    return 1;
}

void add_value_in_header(string const &key, std::map<string, string, Comparator> &headers, const string &str) {
    string old_val = reduce(headers[key], ",", " ,");
    if (count(old_val, ',') > 0 || count(str, ',') > 0) {
        std::vector<string> tmp = split(reduce(str, ",", " ,"), ',');
        for (u_int32_t i = 0; i < tmp.size(); ++i) {
            if (!tmp[i].empty() && old_val.find(tmp[i]) == string::npos)
                old_val += "," + tmp[i];
        }
        headers[key] = old_val;
    } else {
        headers[key] = str;
    }
}