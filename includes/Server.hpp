#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <fcntl.h>
#include <sys/stat.h>
#include "ConfigBlock.hpp"
#include "Location.hpp"

class Server {
	friend class Parser;

	uint32_t address;
	std::string address_str;
	uint32_t port;
	std::vector<std::string> server_names;
	std::map<uint32_t,std::string> error_pages;
	Location server_location;
	std::vector<Location> locations;

	enum { ISFILE, ISDIR, NOTFOUND };

	int checkFile(const std::string &path) const {
		struct stat buf = {};

		if (stat(path.c_str(), &buf) == 0) {
			if (buf.st_mode & S_IFDIR)
				return ISDIR;
			else
				return ISFILE;
		}
		return NOTFOUND;
	}

	Server() : address(), address_str(), port(), server_names(), error_pages(), server_location(), locations() {}

	void printData() const {
		std::cout << "server" << std::endl;
		std::cout << "\taddress: " << address_str << ';' << std::endl;
		std::cout << "\tport: " << port << ';' << std::endl;
		std::cout << "\tserver_name: ";
		for (size_t i = 0, size = server_names.size(); i < size; ++i) {
			if (server_names[i].empty())
				std::cout << "\"\"";
			else
				std::cout << server_names[i];
			if (i + 1 == size)
				std::cout << ';';
			else
				std::cout << ' ';
		}
		std::cout << std::endl;
		if (not error_pages.empty()) {
			std::map<uint32_t,std::string>::const_iterator it = error_pages.begin();

			for (;it != error_pages.end(); ++it)
				std::cout << "\terror_page " << it->first << ' ' << it->second << ';' << std::endl;
		}
		server_location.printData();
		for (size_t i = 0, size = locations.size(); i < size; ++i)
			locations[i].printData();
	}///

public:

	const uint32_t &getAddress() const { return address; }

	const std::string &getAddressStr() const { return address_str; }

	const uint32_t &getPort() const { return port; }

	const std::vector<std::string> &getServerNames() const { return server_names; }

	std::string getErrorPagePath(uint32_t code) const {
		try { return error_pages.at(code); }
		catch (std::exception &) { return ""; }
	}

	const Location &getLocation(const std::string &uri) const {
		std::vector<std::string> path_pool;

		for (size_t pos = uri.size(); pos != 0; pos = uri.find_last_of('/', pos - 1))
			path_pool.push_back(uri.substr(0, pos));
		path_pool.push_back("/");

		for (size_t i = 0, path_size = path_pool.size(); i < path_size; ++i) {
			for (size_t a = 0, loc_size = locations.size(); a < loc_size; ++a) {
				if (locations[a].path == path_pool[i]) {
					return locations[a];
				}
			}
		}
		return server_location;
	}

	std::string getFile(std::string uri, uint8_t method, uint32_t &code, const Location &loc) const {
		std::string ret;

		// std::cout << "loc path " << loc.path << "\n";

		if (loc.path != "/")
			uri.erase(0, loc.path.length());

		if (loc.redirectCheck()) {
			code = loc.redirect.first;
			return loc.redirect.second;
		}

		if (loc.methodCheck(method)) {
			code = 405;
			return getErrorPagePath(code);
		}

		if (loc.isCGI(uri, method))
			ret = loc.cgi_root + uri;
		else
			ret = loc.root + uri;


		std::string::iterator last = ret.end() - 1;
		if (*last == '/') ret.erase(last);

		if (method != PUT) {
			int is_file = checkFile(ret);

			if (is_file == NOTFOUND && !loc.isCGI(uri, method)) {
				code = 404;
				return getErrorPagePath(code);
			}
			else if (is_file == ISDIR) {
				for (size_t i = 0, index_size = loc.indexes.size(); i < index_size; ++i) {
					is_file = checkFile(ret + loc.indexes[i]);
					if (is_file == ISFILE or is_file == ISDIR) {
						return ret + loc.indexes[i];
					}
				}
				if (loc.autoindex) {
					last = ret.end() - 1;
					if (*last != '/') ret.push_back('/');
				}
				else if (is_file == ISDIR) {
					code = 403;
					return getErrorPagePath(code);
				}
				else {
					code = 404;
					return getErrorPagePath(code);
				}
			}
		}
		return ret;
	}

	void checkRoots() {
		if (server_location.root.empty())
			server_location.root = "./www/root/localhost";
		for (size_t i = 0, size = locations.size(); i < size; ++i) {
			if (locations[i].root.empty()) {
				locations[i].root = server_location.root;
			}
		}
	}

	Server(const ConfigBlock &serverBlock) : address(serverBlock.address), address_str(serverBlock.address_str),
											 port(serverBlock.port), server_names(serverBlock.server_name),
											 error_pages(serverBlock.error_pages),
											 server_location(serverBlock), locations() {
		for (size_t i = 0, size = serverBlock.upblock.size(); i < size; ++i)
			locations.push_back(serverBlock.upblock[i]);
		checkRoots();
	}

	Server(const Server &other) : address(other.address), address_str(other.address_str), port(other.port),
								  server_names(other.server_names), error_pages(other.error_pages),
								  server_location(other.server_location), locations(other.locations) {}

	~Server() {}

	Server &operator=(const Server &other) {
		if (this == &other)
			return *this;
		address = other.address;
		address_str = other.address_str;
		port = other.port;
		server_names = other.server_names;
		error_pages = other.error_pages;
		server_location = other.server_location;
		locations = other.locations;
		return *this;
	}

};


#endif
