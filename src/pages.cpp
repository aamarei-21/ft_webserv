#include "pages.hpp"

static void add_header(std::stringstream &body, const string &title) {
    body << "<!DOCTYPE html>\n"
        << "<html lang=\"en\">\n"
        << "<head>\n"
        << "<meta charset=\"UTF-8\">\n"
	    << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        << "<meta name=\"robots\" content=\"none\"/>\n"
        << "<title>C++ HTTP WebServer " << title << "</title>\n"
        << "</head>\n<body>\n";
}

static void add_footer(std::stringstream &body) {
    body << "<em><small>C++ Http WebServer</small></em>\n"
         << "</body>\n</html>";
}

string autoindex(const string &path, const string &url) {
    std::cout << "autoindex for " << path << "\n";
    DIR * dir;
	struct dirent* de;
	struct stat buff;
	std::set<string, Comparator> files;
	std::set<string, Comparator> catalogs;

    dir = opendir(path.c_str());
	if ( dir == NULL ) {
		std::cout << "Forbidden, niht permission!\n";
		return std::string();
	}
    string tmp_path;
	while ( (de = readdir(dir)) ) {
        tmp_path = path + "/" + (string)de->d_name;
		stat(tmp_path.c_str(), &buff);
		if (((buff.st_mode) & S_IFMT) == S_IFREG)
            files.insert(de->d_name);
		else {
			if ((string )de->d_name != "." && (string)de->d_name != "..")
				catalogs.insert(de->d_name);
		}
	}
	closedir(dir);
    
    std::stringstream body;
    add_header(body, "Directory Listing: " + url);
    body << "<h1>Index of " << url << "</h1>\n<table>\n"
        << "<tr>\n<th>Type</th>\n<th>Name</th>\n<th>Size</th>\n"
        << "<th>Last Modified</th>\n</tr>\n\n";
    if (url != "/") {
        string cur_dir = url.substr(0, url.size() - 1);
        body << "<tr>\n<td>Dir</td>\n"
                << "<td><a href=\"" << cur_dir.substr(0, cur_dir.find_last_of("/") + 1) << "\">..</a></td>\n<td></td>\n"
                << "<td></td>\n</tr>\n\n";
    }
	std::set<string>::iterator it;
	for (it = catalogs.begin(); it != catalogs.end(); ++it)
        body << "<tr>\n<td>Dir</td>\n"
            << "<td><a href=\"" << (url + *it + "/\">" + *it) << "/</a></td>\n<td></td>\n"
            << "<td></td>\n</tr>\n\n";
	tmp_path = path + "/";
    for (it = files.begin(); it != files.end(); ++it)
		body << "<tr>\n<td>File</td>\n"
            << "<td><a href=\"" << (url + *it + "\">" + *it) << "</a></td>\n"   //  + "\\"
            << "<td>" << get_file_size(tmp_path + *it) << "</td>\n"
            << "<td>" << get_last_modif(tmp_path + *it) << "</td>\n</tr>\n\n";

    body << "</table>\n";
    add_footer(body);

    return body.str();
}

string gen_error_page(const string &code, const string &status) {
    std::stringstream body;
    add_header(body, "Error page " + code);
    body << "<h1>Error page</h1>\n"
        << "<h1>Error " << code << " " << status << "</h1>\n";
    add_footer(body);

    return body.str();
}

string gen_success_page(const string &code) {
    std::stringstream body;
    add_header(body, "Success page " + code);
    if (code == "200")
        body << "<p>Success 200</p>\n";
    else if (code == "201")
        body << "<p>Create Success 201</p>\n";
    add_footer(body);

    return body.str();
}