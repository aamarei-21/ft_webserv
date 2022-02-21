#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>
#include <vector>
#include <map>

enum { BLOCK, SIMPLE };

template<class T>
class Token {
public:
	std::string name;
	typename T::parseFunc func;
	int type;
	int mayContained;
	int e_cont;

	void add(const std::string &_name, typename T::parseFunc _func, int cont) {
		name = _name;
		func = _func;
		type = SIMPLE;
		e_cont = cont;
	}

	void add(const std::string &_name, typename T::parseFunc _func, int Contained, int cont) {
		name = _name;
		func = _func;
		mayContained = Contained;
		type = BLOCK;
		e_cont = cont;
	}

	Token() : name(), func(NULL), type(-1), mayContained(0) {}

	Token(const Token &other) : name(other.name), func(other.func), type(other.type), mayContained(other.mayContained), e_cont(other.e_cont) {}

	~Token() {}

	bool operator==(const std::string &str) const {
		if (str == name)
			return true;
		return false;
	}

	bool operator==(int e_type) const {
		if (e_type == type)
			return true;
		return false;
	}

	Token &operator=(const Token &other) {
		if (this == &other)
			return *this;
		name = other.name;
		func = other.func;
		type = other.type;
		mayContained = other.mayContained;
		e_cont = other.e_cont;
		return *this;
	}
};


#endif //TOKEN_HPP
