#ifndef PARSER_HPP
#define PARSER_HPP

#define DEFAULT_CONF_PATH "./default"

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include "ConfigBlock.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include <stack>

void trim2(std::string &str, const std::string &set) {
	if (str.empty() or set.empty()) return ;
	size_t first = str.find_first_not_of(set), last;

	str.erase(0, first);
	if (str.empty()) return ;
	last = str.find_last_not_of(set) + 1;
	str.erase(last);
}

class Parser {
	std::vector< std::vector< std::string > > split_lines;

	void preparser(std::vector<std::string> &buffer) {
		for (size_t i = 0, sharp_pos, size = buffer.size(); i < size; ++i) {
			if ((sharp_pos = buffer[i].find('#')) != std::string::npos)
				buffer[i].erase(sharp_pos);
			trim2(buffer[i], " \t\r");
			if (buffer[i].empty()) {
				buffer.erase(buffer.begin() + i);
				--i;
				--size;
			}
			else split_lines.push_back(split(buffer[i], ' '));
		}
	}

	void parse(ConfigBlock &mainBlock, size_t &pos, int indicator) {
		ConfigBlock::parseFunc func;
		size_t i, size;

		mainBlock.type = indicator;
		for (size = split_lines.size(); pos < size; ++pos) {
			i = check_token(split_lines[pos], mainBlock.tokens);
			if (mainBlock.tokens[i] == BLOCK) {
				if ((indicator == SERVER_B and mainBlock.tokens[i].e_cont == SERVER) or indicator == LOCATION_B) { --pos; return; }
				mainBlock.upblock.resize(mainBlock.upblock.size() + 1);
				pos++;
				parse(mainBlock.upblock.back(), pos, indicator + 1);
			}
			else if (mainBlock.tokens[i] == SIMPLE) {
				func = mainBlock.tokens[i].func;
				(mainBlock.*func)(split_lines[pos]);
			}
			else break ;
		}
	}

	void syntax_check(const std::vector< Token<ConfigBlock> > &tokens, size_t pos) {
		size_t i, size;
		Token<ConfigBlock> tok;

		tok.add("main", NULL, SERVER, MAIN);
		for (size = split_lines.size(); pos < size; ++pos) {
			i = check_token(split_lines[pos], tokens);
			if (tokens[i] == BLOCK) {
				if (tok.e_cont == MAIN && !(tok.mayContained & tokens[i].e_cont))
					throw std::runtime_error("block error: " + tok.name + " cannot contain " + tokens[i].name);
				tok = tokens[i];
			}
			else if (tokens[i] == SIMPLE) {
				if (tok.e_cont == MAIN) throw std::runtime_error("syntax error: " + tokens[i].name + " cannot be outside block");
				if (!(tok.mayContained & tokens[i].e_cont))
					throw std::runtime_error("block error: " + tok.name + " cannot contain " + tokens[i].name);
				simple_check(pos);
			}
			else break ;
		}
	}

	size_t check_token(const std::vector<std::string> &line, const std::vector<Token<ConfigBlock> > &tokens) {
		for (size_t i = 0, size = tokens.size(); i < size; ++i)
			if (tokens[i] == line[0]) return i;
		throw std::runtime_error("syntax error: unexpected token");
	}

	void simple_check(size_t pos) {
		std::string &tmp = split_lines[pos].back();
		size_t last_pos = tmp.size() - 1;

		if (tmp.rfind(';') != last_pos) throw std::runtime_error("syntax error: expected \';\'");
		tmp.erase(last_pos);
	}

	Parser() : split_lines() {}

public:

	void getContent(std::vector< Server > &servers) {
		ConfigBlock mainBlock;
		size_t pos = 0;

		syntax_check(mainBlock.tokens, pos);
		parse(mainBlock, pos, 0);
		mainBlock.check_unique();
		for (size_t i = 0, size = mainBlock.upblock.size(); i < size; ++i)
			servers.push_back(mainBlock[i]);
		// for (size_t i = 0, size = servers.size(); i < size; ++i)
		// 	servers[i].printData();

	}

	explicit Parser(char *path = (char *)DEFAULT_CONF_PATH) {
		std::ifstream config(path);

		if (not config.is_open()) throw std::ios_base::failure("parser error: cannot open config file");

		std::vector<std::string> split_buffer;

		for (std::string buf; std::getline(config, buf, '\n');)
			split_buffer.push_back(buf);
		config.close();
		preparser(split_buffer);
	}

	Parser(const Parser &other) : split_lines(other.split_lines) {}

	~Parser() {}

	Parser &operator=(const Parser &other) {
		if (this == &other)
			return *this;
		split_lines = other.split_lines;
		return *this;
	}

};

#endif
