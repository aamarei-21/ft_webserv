#ifndef CONFIGBLOCK_HPP
#define CONFIGBLOCK_HPP

#include <vector>
#include <cstdlib>
#include "Token.hpp"
#include <arpa/inet.h>
#include <sstream>

enum {
	NONE = 0x0,
	ADDRESS = 0x1,
	PORT = 0x2,
	SERVER_NAME = 0x4,
	ROOT = 0x8,
	INDEX = 0x10,
	AUTOINDEX = 0x20,
	MAX_BODY = 0x40,
	ERROR_PAGE = 0x80,
	PATH = 0x100,
	SERVER = 0x200,
	LOCATION = 0x400,
	MAIN = 0x800,
	METHOD = 0x1000,
	CGI = 0x2000,
	CGI_ROOT = 0x4000,
	UPLOAD = 0x8000,
	REDIR = 0x10000
};

enum {
	GET = 0x1,
	POST = 0x2,
	PUT = 0x4,
	DELETE = 0x8,
	HEAD = 0x10,
	CONNECT = 0x20
};

enum {
	MAIN_B,
	SERVER_B,
	LOCATION_B
};

class ConfigBlock {
	friend class Parser;
	friend class Server;
	friend class Location;
	friend class Token<ConfigBlock>;

	typedef void (ConfigBlock::*parseFunc)(const std::vector<std::string> &);

	int type;

	uint32_t address;
	std::string address_str;
	uint32_t port;
	std::vector< std::string > server_name;
	std::map< uint32_t,std::string > error_pages;
	size_t client_max_body_size;
	std::string root;
	bool autoindex;
	std::vector< std::string > indexes;
	std::string path;
	uint8_t methods;
	std::map<std::string, std::pair<std::string,uint8_t> > cgi;
	std::string cgi_root;
	std::string upload_path;
	std::pair< uint32_t,std::string > redirect;
	std::vector< ConfigBlock > upblock;

	std::vector< Token<ConfigBlock> > tokens;

	static void AddressToString(uint32_t addr, std::string &strbuf) {
		if (addr == 0) {
			strbuf = "0.0.0.0";
			return ;
		}
		else if ((int)addr == -1) {
			strbuf = "255.255.255.255";
			return ;
		}
		std::stringstream strstream;

		for (size_t i = 0; i < 4; ++i) {
			strstream << (int)(uint8_t)(addr >> i * 8);
			if (i != 3)
				strstream << ".";
		}
		strbuf = strstream.str();
	}

public:

	void check_unique() const {
		size_t i, size;

		if (type == 0) {
			for (i = 0, size = upblock.size(); i < size; ++i) {
				// for (size_t i2 = i + 1; i2 < size; ++i2)
				// 	if (upblock[i].port == upblock[i2].port) throw std::runtime_error("server error: servers cannot have equal ports");
				upblock[i].check_unique();
			}
		}
		else if (type == 1) {
			for (i = 0, size = upblock.size(); i < size; ++i) {
				for (size_t i2 = i + 1; i2 < size; ++i2)
					if (upblock[i].path == upblock[i2].path) throw std::runtime_error("location error: locations cannot have equal path");
				upblock[i].check_unique();
			}
		}
	}

	void parseAddress(const std::vector<std::string> &line) {
		if (line.size() != 2) throw std::runtime_error("parse error: address: too much arguments");
		if (line.back().empty()) throw std::runtime_error("parse error: address: empty field");

		const std::string &tmp = line.back();

		address = inet_addr(tmp.c_str());
		if ((int)address == -1 and tmp != "255.255.255.255") throw std::runtime_error("parse error: address: invalid address");
		AddressToString(address, address_str);
	}

	void parsePort(const std::vector<std::string> &line) {
		if (line.size() != 2) throw std::runtime_error("parse error: port: too much arguments");
		if (line.back().empty()) throw std::runtime_error("parse error: port: empty field");

		const std::string &tmp = line.back();

		for (size_t i = 0, size = tmp.size(); i < size; ++i)
			if (not isdigit(tmp[i])) throw std::runtime_error("parse error: port: expected only digit");
		port = strtol(tmp.c_str(), NULL, 10);
	}

	void parseServerName(const std::vector<std::string> &line) {
		server_name.clear();

		server_name.insert(server_name.begin(), line.begin() + 1, line.end());
	}

	void parseRoot(const std::vector<std::string> &line) {
		if (line.size() != 2) throw std::runtime_error("parse error: root: too much arguments");
		if (line.back().empty()) throw std::runtime_error("parse error: root: empty field");

		root = line.back();
		if (root == "/") return ;
		std::string::iterator last_char = root.end() - 1;

		if (*last_char == '/') root.erase(last_char);
		if (root[0] != '/') root.insert(root.begin(), '/');

	}

	void parseIndex(const std::vector<std::string> &line) {
		if (line.back().empty()) throw std::runtime_error("parse error: index: empty field");

		indexes.clear();

		indexes.insert(indexes.begin(), line.begin() + 1, line.end());

		for (size_t i = 0, size = indexes.size(); i < size; ++i) {
			std::string::iterator last_char = indexes[i].end() - 1;

			if (*last_char == '/') throw std::runtime_error("parse error: index: index cannot be a directory");///
			if (indexes[i][0] != '/') indexes[i].insert(indexes[i].begin(), '/');
		}
	}

	void parseAutoindex(const std::vector<std::string> &line) {
		if (line.size() != 2) throw std::runtime_error("parse error: autoindex: too much arguments");
		if (line.back() == "on") autoindex = true;
		else if (line.back() == "off") autoindex = false;
		else throw std::runtime_error("parse error: autoindex: invalid parameter");
	}

	void parseClientMaxBodySize(const std::vector<std::string> &line) {
		if (line.size() != 2) throw std::runtime_error("parse error: client max body size: too much arguments");
		if (line.back().empty()) throw std::runtime_error("parse error: client max body size: empty field");

		const std::string &tmp = line.back();

		for (size_t i = 0, size = tmp.size(); i < size; ++i)
			if (not isdigit(tmp[i])) throw std::runtime_error("parse error: client max body size: expected only digit");
		client_max_body_size = strtol(tmp.c_str(), NULL, 10);
	}

	void parseErrorPage(const std::vector<std::string> &line) {
		if (line.back().empty()) throw std::runtime_error("parse error: error page: empty field");

		size_t i, a, size, sizel;
		std::string tmpl = line.back();
		std::string::iterator last_char = tmpl.end() - 1;

		if (*last_char == '/') throw std::runtime_error("parse error: error page: error page cannot be a directory");
		if (tmpl[0] != '/') tmpl.insert(tmpl.begin(), '/');
		for (i = 1, size = line.size() - 1; i < size; ++i) {
			for (a = 0, sizel = line[i].size(); a < sizel; ++a) {
				if (not isdigit(line[i][a])) throw std::runtime_error("parse error: error page: expected only digit");
			}
			error_pages[strtol(line[i].c_str(), NULL, 10)] = tmpl;
		}
	}

	void parsePath(const std::vector<std::string> &line) {///
		if (line.size() != 2) throw std::runtime_error("parse error: path: too much arguments");
		if (line.back().empty()) throw std::runtime_error("parse error: location path: empty field");

		path = line.back();
		if (path == "/") return ;

		std::string::iterator last_char = path.end() - 1;

		if (*last_char == '/') path.erase(last_char);
		if (path[0] != '/') path.insert(path.begin(), '/');
	}

	void parseMethod(const std::vector<std::string> &line) {
		if (line.back().empty()) throw std::runtime_error("parse error: method: empty field");
		for (size_t i = 1, size = line.size(); i < size; ++i) {
			if (line[i] == "GET") methods |= GET;
			else if (line[i] == "POST") methods |= POST;
			else if (line[i] == "PUT") methods |= PUT;
			else if (line[i] == "DELETE") methods |= DELETE;
			else throw std::runtime_error("parse error: method: invalid parameter");
		}
	}

	void parseCGI(const std::vector<std::string> &line) {
		if (line.size() < 3) throw std::runtime_error("parse error: cgi: invalid parameter");
		if (line.back().empty()) throw std::runtime_error("parse error: cgi: empty field");

		if (line[1].size() == 1 or line[1][0] != '.') throw std::runtime_error("parse error: cgi: invalid extension parameter");

		std::string tmp = line.back();

		std::string::iterator last_char = tmp.end() - 1;

		if (*last_char == '/') throw std::runtime_error("parse error: cgi: cgi path cannot be a directory");///
		if (tmp[0] != '/') tmp.insert(tmp.begin(), '/');

		uint8_t tmp_methods = 0;

		if (line.size() == 3) { tmp_methods |= GET; }
		else {
			for (size_t i = 2, size = line.size() - 1; i < size; ++i) {
				if (line[i] == "GET") tmp_methods |= GET;
				else if (line[i] == "POST") tmp_methods |= POST;
				else if (line[i] == "PUT") tmp_methods |= PUT;
				else if (line[i] == "DELETE") tmp_methods |= DELETE;
				else throw std::runtime_error("parse error: cgi: invalid allow method parameter");
			}
		}
		cgi.insert(std::make_pair(line[1], std::make_pair(tmp, tmp_methods)));
	}

	void parseCGIroot(const std::vector<std::string> &line) {
		if (line.size() != 2) throw std::runtime_error("parse error: cgi root: too much arguments");
		if (line.back().empty()) throw std::runtime_error("parse error: cgi root: empty field");
		cgi_root = line.back();

		std::string::iterator last_char = cgi_root.end() - 1;

		if (*last_char == '/') cgi_root.erase(last_char);
		if (cgi_root[0] != '/') cgi_root.insert(cgi_root.begin(), '/');
	}

	void parseUpload(const std::vector<std::string> &line) {
		if (line.size() != 2) throw std::runtime_error("parse error: upload: too much arguments");
		if (line.back().empty()) throw std::runtime_error("parse error: upload: empty field");

		upload_path = line.back();
		std::string::iterator last_char = upload_path.end() - 1;

		if (*last_char == '/') upload_path.erase(last_char);
		if (upload_path[0] != '/') upload_path.insert(upload_path.begin(), '/');
	}

	void parseRedirect(const std::vector<std::string> &line) {
		if (line.size() != 3) throw std::runtime_error("parse error: redirect: too much arguments");
		if (line.back().empty()) throw std::runtime_error("parse error: redirect: empty field");

		const std::string &tmp = line[1];

		for (size_t i = 0, size = tmp.size(); i < size; ++i)
			if (not isdigit(tmp[i])) throw std::runtime_error("parse error: redirect: expected only digit");
		redirect.first = strtol(tmp.c_str(), NULL, 10);
		redirect.second = line[2];
	}

	ConfigBlock() : type(0), address(inet_addr("127.0.0.1")), address_str("127.0.0.1"),
					port(80), server_name(1, ""), error_pages(), client_max_body_size(0),
					root(), autoindex(false), indexes(1, "/index.html"),
					path("/"), methods(GET), cgi(), cgi_root(),
					upload_path(), redirect(std::make_pair(-1, "")), upblock(), tokens(16) {
		tokens[0].add("address", &ConfigBlock::parseAddress, ADDRESS);
		tokens[1].add("port", &ConfigBlock::parsePort, PORT);
		tokens[2].add("server_name", &ConfigBlock::parseServerName, SERVER_NAME);
		tokens[3].add("root", &ConfigBlock::parseRoot, ROOT);
		tokens[4].add("index", &ConfigBlock::parseIndex, INDEX);
		tokens[5].add("autoindex", &ConfigBlock::parseAutoindex, AUTOINDEX);
		tokens[6].add("client_max_body_size", &ConfigBlock::parseClientMaxBodySize, MAX_BODY);
		tokens[7].add("error_page", &ConfigBlock::parseErrorPage, ERROR_PAGE);
		tokens[8].add("path", &ConfigBlock::parsePath, PATH);
		tokens[9].add("method", &ConfigBlock::parseMethod, METHOD);
		tokens[10].add("cgi", &ConfigBlock::parseCGI, CGI);
		tokens[11].add("cgi_root", &ConfigBlock::parseCGIroot, CGI_ROOT);
		tokens[12].add("upload", &ConfigBlock::parseUpload, UPLOAD);
		tokens[13].add("redirect", &ConfigBlock::parseRedirect, REDIR);
		tokens[14].add("server", NULL, ADDRESS | PORT | SERVER_NAME | ROOT | INDEX | AUTOINDEX | MAX_BODY | ERROR_PAGE | REDIR | LOCATION, SERVER);
		tokens[15].add("location", NULL, PATH | ROOT | INDEX | AUTOINDEX | MAX_BODY | METHOD | CGI | CGI_ROOT | UPLOAD | REDIR, LOCATION);
	}

	ConfigBlock(const ConfigBlock &other) : type(other.type), address(other.address), address_str(other.address_str),
											port(other.port), server_name(other.server_name), error_pages(other.error_pages),
											client_max_body_size(other.client_max_body_size), root(other.root),
											autoindex(other.autoindex), indexes(other.indexes), path(other.path),
											methods(other.methods), cgi(other.cgi),
											cgi_root(other.cgi_root), upload_path(other.upload_path), redirect(other.redirect),
											upblock(other.upblock), tokens(other.tokens) {}

	~ConfigBlock() {}

	const ConfigBlock &operator[](int index) const { return upblock[index]; }

	ConfigBlock &operator=(const ConfigBlock &other) {
		if (this == &other)
			return *this;
		type = other.type;
		address = other.address;
		address_str = other.address_str;
		port = other.port;
		server_name = other.server_name;
		error_pages = other.error_pages;
		client_max_body_size = other.client_max_body_size;
		root = other.root;
		autoindex = other.autoindex;
		indexes = other.indexes;
		path = other.path;
		methods = other.methods;
		cgi = other.cgi;
		cgi_root = other.cgi_root;
		upload_path = other.upload_path;
		redirect = other.redirect;
		upblock = other.upblock;
		tokens = other.tokens;
		return *this;
	}
};


#endif
