#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <map>
#include <iostream>

class Server;

class Location {
	friend class Server;

	std::string path;
	std::string root;
	bool autoindex;
	size_t client_max_body_size;
	std::vector<std::string> indexes;
	uint8_t methods;
	std::map<std::string, std::pair<std::string,uint8_t> > cgi;
	std::string cgi_root;
	std::string upload_path;
	std::pair<uint32_t,std::string> redirect;

	void printData() const {
		std::cout << "location" << std::endl;
		std::cout << "\tpath: " << path << ';' << std::endl;
		std::cout << "\troot: " << root << ';' << std::endl;
		std::cout << "\tautoindex " << (autoindex ? "on" : "off") << ';' << std::endl;
		std::cout << "\tclient_max_body_size " << client_max_body_size << ';' << std::endl;
		std::cout << "\tmethod ";
		if (methods & GET) std::cout << "GET ";
		if (methods & POST) std::cout << "POST ";
		if (methods & PUT) std::cout << "PUT ";
		if (methods & DELETE) std::cout << "DELETE ";
		std::cout << "\b;" << std::endl;
		std::cout << "\tindex ";
		for (size_t i = 0; i < indexes.size(); ++i)
			std::cout << indexes[i] << ' ';
		std::cout << "\b;" << std::endl;
		if (not cgi.empty()) {
			std::cout << "#Location may handle cgi with this parameters:" << std::endl;
			std::map<std::string,std::pair<std::string,uint8_t> >::const_iterator it = cgi.begin();
			std::map<std::string,std::pair<std::string,uint8_t> >::const_iterator ite = cgi.end();
			for (int i = 1; it != ite; ++it, ++i) {
				std::cout << "\tcgi " << it->first << ' ';
				if (it->second.second & GET) std::cout << "GET ";
				if (it->second.second & POST) std::cout << "POST ";
				if (it->second.second & PUT) std::cout << "PUT ";
				if (it->second.second & DELETE) std::cout << "DELETE ";
				std::cout << "\b ";
				std::cout << it->second.first << ";" << std::endl;;
			}
			std::cout << "\tcgi_root " << cgi_root << ';' << std::endl;
		}
		if (not upload_path.empty())
			std::cout << "\tuplload " << upload_path << ';' << std::endl;
		if (redirectCheck()) {
			std::cout << "\tredirect " << redirect.first << ' ' << redirect.second << ';' << std::endl;
		}
	}

public:

	const std::string &getRoot() const { return root; }

	bool redirectCheck() const { return (int)redirect.first != -1; }

	bool methodCheck(uint8_t method) const { return !(methods & method); }

	bool isAotuindex() const { return autoindex; }

	size_t getMaxBody() const { return client_max_body_size; }

	std::string getUploadPath(const std::string &uri) const {
		if (uri.length() < path.length())
			return upload_path;
		return upload_path + uri.substr(path.length());
	}

	const std::string &getCGIPath(const std::string &url) const { return cgi.at(url.substr(url.find_last_of('.'))).first; }

	std::string getAllow() const {
		std::stringstream method_str;
		method_str << "GET";
		if (methods & POST) method_str << ", POST";
		if (methods & PUT) method_str << ", PUT";
		if (methods & DELETE) method_str << ", DELETE";
		return method_str.str();
	}

	bool isCGI(std::string &uri, uint8_t method) const {
		if (cgi.empty()) return false;
		try { if (cgi.at(uri.substr(uri.find_last_of('.'))).second & method) return true; }
		catch (std::exception &) { return false; }
		return false;
	}

	Location(const ConfigBlock &other) : path(other.path), root(other.root), autoindex(other.autoindex),
										 client_max_body_size(other.client_max_body_size), indexes(other.indexes),
										 methods(other.methods), cgi(other.cgi), cgi_root(other.cgi_root),
										 upload_path(other.upload_path), redirect(other.redirect) {}

	explicit Location() : path(), root(), autoindex(), client_max_body_size(), indexes(), methods(), cgi(),
						cgi_root(), upload_path(), redirect() {}

	Location(const Location &other) : path(other.path), root(other.root), autoindex(other.autoindex),
									client_max_body_size(other.client_max_body_size), indexes(other.indexes),
									methods(other.methods), cgi(other.cgi), cgi_root(other.cgi_root),
									upload_path(other.upload_path), redirect(other.redirect) {}

	~Location() {};

	Location &operator=(const Location &other) {
		if (this == &other)
			return *this;
		path = other.path;
		root = other.root;
		autoindex = other.autoindex;
		client_max_body_size = other.client_max_body_size;
		indexes = other.indexes;
		methods = other.methods;
		cgi = other.cgi;
		cgi_root = other.cgi_root;
		upload_path = other.upload_path;
		redirect = other.redirect;
		return *this;
	}

};

#endif
