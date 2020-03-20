#include <cctype>
#include <iostream>
#include <cstdlib>

#include "lex.hh"
#include "strutils.hh"

bool is_separator(char c) {
	if (c == ' ' || c == '(' || c == ')'
		|| c == '[' || c == ']' || c == '&' || c == '?'
		|| c == '=' || c == ',' || c == ':' || c == '.'
		|| c == '+' || c == '-' || c == '*' || c == '/'
		|| c == '%' || c == '!' || c == '>' || c == '<') {
		return true;
	}
	
	return false;
}

bool is_int(std::string s) {
	int index = 0;
	
	for (char c : s) {
		if (c == '-') {
			if (index > 0) {
				return false;
			}
			
			++index;
			continue;
		}
		
		if (!isdigit(c))
			return false;
			
		++index;
	}
	return true;
}

bool is_dec(std::string s) {
	for (char c : s) {
		if (!isdigit(c) && c != '.')
			return false;
	}
	
	return true;
}

bool is_hex(std::string s) {
	if (s.length() < 3)
		return false;
		
	if (s[0] != '0' || s[1] != 'x') {
		return false;
	}
	
	for (int i = 2; i<s.length(); i++) {
		char c = s[i];
		
		if (!isxdigit(c))
			return false;
	}
	
	return true;
}

bool is_last_op(std::vector<Token> tokens) {
	auto t = tokens.at(tokens.size()-1);
	
	switch (t.type) {
		case TokenType::ASSIGN:
		case TokenType::PLUS:
		case TokenType::MINUS:
		case TokenType::MUL:
		case TokenType::DIV:
		case TokenType::MOD:
		case TokenType::D_PLUS:
		case TokenType::D_MUL:
		case TokenType::AND:
		case TokenType::OR:
		case TokenType::XOR: return true;
	}
	
	return false;
}

//Tokenize a string
std::vector<Token> tokenize(std::string line) {
	line += " ";	//No purpose, makes my life easier :)
	
	std::vector<Token> tokens;
	std::string current = "";
	bool in_quote = false;
	bool skip_next = false;
	char last = 0;
	
	for (int i = 0; i<line.length(); i++) {
		char c = line[i];
	
		if (skip_next) {
			skip_next = false;
			continue;
		}
	
		//First, check against double quotes
		if ((c == '\"' || c == '\'') && in_quote) {
			Token t;
			t.type = TokenType::STRING;
			t.id = current.c_str();
			
			if (c == '\'') {
				t.type = TokenType::CHAR;
				if (current.length() > 1) {
					std::cout << "Lexical Error: A char literal must be of length 1." << std::endl;
					std::cout << "> " << line << std::endl;
					std::exit(1);
				}
			}
			
			tokens.push_back(t);
			
			current = "";
			in_quote = false;
			continue;
		} else if (c == '\"' || c == '\'') {
			in_quote = true;
			continue;
		} else if (in_quote) {
			current += c;
			continue;
			
		//If we have a comment, we drop the rest of the line
		} else if (c == '#') {
			return tokens;
			
		//Check to see if we have a separator
		//If we do, that signifies we have at least one
		// new token
		} else if (is_separator(c)) {
			Token t;
			
			//Do a quick check on the dot operator
			if (c == '.') {
				char nx = line[i+1];
				
				if (is_int(current) && isdigit(nx)) {
					current += c;
					continue;
				}
			}
			
			if (c == '-' && is_last_op(tokens)) {
				current += c;
				continue;
			}
			
			//Check for keywords
			//Function keywords
			if (current == "func") {
				t.type = TokenType::FUNC_DEC;
				tokens.push_back(t);
			} else if (current == "extern") {
				t.type = TokenType::EXTERN;
				tokens.push_back(t);
			} else if (current == "global") {
				t.type = TokenType::GLOBAL;
				tokens.push_back(t);
			} else if (current == "end") {
				t.type = TokenType::END;
				tokens.push_back(t);
			} else if (current == "return") {
				t.type = TokenType::RETURN;
				tokens.push_back(t);
				
			//Structure
			} else if (current == "struct") {
				t.type = TokenType::STRUCT;
				tokens.push_back(t);
				
			//Variable types
			} else if (current == "byte") {
				t.type = TokenType::T_BYTE;
				tokens.push_back(t);
			} else if (current == "char") {
				t.type = TokenType::T_CHAR;
				tokens.push_back(t);
			} else if (current == "short") {
				t.type = TokenType::T_SHORT;
				tokens.push_back(t);
			} else if (current == "bool") {
				t.type = TokenType::T_BOOL;
				tokens.push_back(t);
			} else if (current == "str") {
				t.type = TokenType::T_STR;
				tokens.push_back(t);
				
			//Integer types
			} else if (current == "int") {
				t.type = TokenType::T_INT;
				tokens.push_back(t);
			} else if (current == "int64") {
				t.type = TokenType::INT_64;
				tokens.push_back(t);
			} else if (current == "int128") {
				t.type = TokenType::INT_128;
				tokens.push_back(t);
			} else if (current == "int256") {
				t.type = TokenType::INT_256;
				tokens.push_back(t);
				
			//Floating-point types
			} else if (current == "float") {
				t.type = TokenType::T_FLOAT;
				tokens.push_back(t);
			} else if (current == "float64") {
				t.type = TokenType::FLOAT_64;
				tokens.push_back(t);
			} else if (current == "float128") {
				t.type = TokenType::FLOAT_128;
				tokens.push_back(t);
			} else if (current == "float256") {
				t.type = TokenType::FLOAT_256;
				tokens.push_back(t);
				
			//Double types
			} else if (current == "double") {
				t.type = TokenType::T_DOUBLE;
				tokens.push_back(t);
			} else if (current == "double128") {
				t.type = TokenType::DOUBLE_128;
				tokens.push_back(t);
			} else if (current == "double256") {
				t.type = TokenType::DOUBLE_256;
				tokens.push_back(t);
				
			//Unsigned integers
			} else if (current == "uint") {
				t.type = TokenType::T_UINT;
				tokens.push_back(t);
			} else if (current == "uint64") {
				t.type = TokenType::UINT_64;
				tokens.push_back(t);
			} else if (current == "uint128") {
				t.type = TokenType::UINT_128;
				tokens.push_back(t);
			} else if (current == "uint256") {
				t.type = TokenType::UINT_256;
				tokens.push_back(t);
				
			//Conditional stuff
			} else if (current == "if") {
				t.type = TokenType::IF;
				tokens.push_back(t);
			} else if (current == "elif") {
				t.type = TokenType::ELIF;
				tokens.push_back(t);
			} else if (current == "else") {
				t.type = TokenType::ELSE;
				tokens.push_back(t);
				
			//Loops
			} else if (current == "while") {
				t.type = TokenType::WHILE;
				tokens.push_back(t);
			} else if (current == "loop") {
				t.type = TokenType::LOOP;
				tokens.push_back(t);
			} else if (current == "foreach") {
				t.type = TokenType::FOREACH;
				tokens.push_back(t);
				
			//Booleans
			} else if (current == "true") {
				t.type = TokenType::B_TRUE;
				tokens.push_back(t);
			} else if (current == "false") {
				t.type = TokenType::B_FALSE;
				tokens.push_back(t);
				
			//Check the dot operator
			/*} else if (c == '.') {
				char nx = line[i+1];
				
				if (is_int(current) && isdigit(nx)) {
					current += c;
					continue;
				}*/
				
			//If no keyword, and its not empty, then we have an identifier
			} else if (current.size() > 0) {
				if (is_int(current)) {
					t.type = TokenType::NO;
				} else if (is_dec(current)) {
					t.type = TokenType::DEC;
				} else if (is_hex(current)) {
					t.type = TokenType::HEX;
				} else {
					t.type = TokenType::ID;
				}
				
				t.id = current;
				tokens.push_back(t);
			}
			
			current = "";
			
			//Check to see if the separator is also a token
			if (c == '.') {
				Token t;
				t.type = TokenType::DOT;
				tokens.push_back(t);
			} else if (c == '(') {
				Token t;
				t.type = TokenType::LEFT_PAREN;
				tokens.push_back(t);
			} else if (c == ')') {
				Token t;
				t.type = TokenType::RIGHT_PAREN;
				tokens.push_back(t);
			} else if (c == '[') {
				Token t;
				t.type = TokenType::L_BRACKET;
				tokens.push_back(t);
			} else if (c == ']') {
				Token t;
				t.type = TokenType::R_BRACKET;
				tokens.push_back(t);
			} else if (c == ',') {
				Token t;
				t.type = TokenType::COMMA;
				tokens.push_back(t);
			} else if (c == ':') {
				Token t;
				t.type = TokenType::COLON;
				tokens.push_back(t);
			} else if (c == '=' && line[i+1] == '=') {
				Token t;
				t.type = TokenType::EQUALS;
				tokens.push_back(t);
				skip_next = true;
			} else if (c == '=') {
				Token t;
				t.type = TokenType::ASSIGN;
				tokens.push_back(t);
			} else if (c == '!' && line[i+1] == '=') {
				Token t;
				t.type = TokenType::NOT_EQUAL;
				tokens.push_back(t);
				skip_next = true;
			} else if (c == '!') {
				Token t;
				t.type = TokenType::NOT;
				tokens.push_back(t);
			} else if (c == '>' && line[i+1] == '=') {
				Token t;
				t.type = TokenType::GREATER_EQ;
				tokens.push_back(t);
				skip_next = true;
			} else if (c == '>') {
				Token t;
				t.type = TokenType::GREATER;
				tokens.push_back(t);
			} else if (c == '<' && line[i+1] == '=') {
				Token t;
				t.type = TokenType::LESS_EQ;
				tokens.push_back(t);
				skip_next = true;
			} else if (c == '<') {
				Token t;
				t.type = TokenType::LESS;
				tokens.push_back(t);
			} else if (c == '+' && line[i+1] == '+') {
				Token t;
				t.type = TokenType::D_PLUS;
				tokens.push_back(t);
				skip_next = true;
			} else if (c == '+') {
				Token t;
				t.type = TokenType::PLUS;
				tokens.push_back(t);
			} else if (c == '-') {
				Token t;
				t.type = TokenType::MINUS;
				tokens.push_back(t);
			} else if (c == '*' && line[i+1] == '*') {
				Token t;
				t.type = TokenType::D_MUL;
				tokens.push_back(t);
				skip_next = true;
			} else if (c == '*') {
				Token t;
				t.type = TokenType::MUL;
				tokens.push_back(t);
			} else if (c == '/') {
				Token t;
				t.type = TokenType::DIV;
				tokens.push_back(t);
			} else if (c == '%') {
				Token t;
				t.type = TokenType::MOD;
				tokens.push_back(t);
			} else if (c == ',') {
				Token t;
				t.type = TokenType::COMMA;
				tokens.push_back(t);
				
			//Logical operators
			} else if (c == '&' && line[i+1] == '&') {
				Token t;
				t.type = TokenType::AND;
				tokens.push_back(t);
				skip_next = true;
			} else if (c == '?' && line[i+1] == '?') {
				Token t;
				t.type = TokenType::XOR;
				tokens.push_back(t);
				skip_next = true;
			} else if (c == '?') {
				Token t;
				t.type = TokenType::OR;
				tokens.push_back(t);
				skip_next = true;
			}
		} else {
			current += c;
		}
		
		last = c;
	}
	
	return tokens;
}

//Convert a string to a token
TokenType str2type(std::string in) {
	if (in == "ID") return TokenType::ID;
	else if (in == "FUNC_DEC") return TokenType::FUNC_DEC;
	else if (in == "(") return TokenType::LEFT_PAREN;
	else if (in == ")") return TokenType::RIGHT_PAREN;
	else if (in == "STRING") return TokenType::STRING;
	else if (in == "END") return TokenType::END;
	else if (in == ",") return TokenType::COMMA;
	else if (in == "DOT") return TokenType::DOT;
	else if (in == "ASSIGN") return TokenType::ASSIGN;
	else if (in == "T_BYTE") return TokenType::T_BYTE;
	else if (in == "T_CHAR") return TokenType::T_CHAR;
	else if (in == "T_SHORT") return TokenType::T_SHORT;
	else if (in == "T_INT") return TokenType::T_INT;
	else if (in == "T_FLOAT") return TokenType::T_FLOAT;
	else if (in == "T_BOOL") return TokenType::T_BOOL;
	else if (in == "T_STR") return TokenType::T_STR;
	else if (in == "NO") return TokenType::NO;
	else if (in == "CHAR") return TokenType::CHAR;
	else if (in == "DEC") return TokenType::DEC;
	else if (in == "B_TRUE") return TokenType::B_TRUE;
	else if (in == "B_FALSE") return TokenType::B_FALSE;
	else if (in == "PLUS") return TokenType::PLUS;
	else if (in == "MINUS") return TokenType::MINUS;
	else if (in == "MUL") return TokenType::MUL;
	else if (in == "DIV") return TokenType::DIV;
	else if (in == "MOD") return TokenType::MOD;
	else if (in == "COLON") return TokenType::COLON;
	else if (in == "EXTERN") return TokenType::EXTERN;
	else if (in == "IF") return TokenType::IF;
	else if (in == "==") return TokenType::EQUALS;
	else if (in == "!=") return TokenType::NOT_EQUAL;
	else if (in == "ELIF") return TokenType::ELIF;
	else if (in == "ELSE") return TokenType::ELSE;
	else if (in == "NOT") return TokenType::NOT;
	else if (in == "GREATER") return TokenType::GREATER;
	else if (in == "LESS") return TokenType::LESS;
	else if (in == "GREATER_EQ") return TokenType::GREATER_EQ;
	else if (in == "LESS_EQ") return TokenType::LESS_EQ;
	else if (in == "WHILE") return TokenType::WHILE;
	else if (in == "RETURN") return TokenType::RETURN;
	else if (in == "L_BRACKET") return TokenType::L_BRACKET;
	else if (in == "R_BRACKET") return TokenType::R_BRACKET;
	else if (in == "D_PLUS") return TokenType::D_PLUS;
	else if (in == "D_MUL") return TokenType::D_MUL;
	else if (in == "FLOAT_128") return TokenType::FLOAT_128;
	else if (in == "FLOAT_256") return TokenType::FLOAT_256;
	else if (in == "INT_128") return TokenType::INT_128;
	else if (in == "INT_256") return TokenType::INT_256;
	else if (in == "HEX") return TokenType::HEX;
	else if (in == "AND") return TokenType::AND;
	else if (in == "OR") return TokenType::OR;
	else if (in == "XOR") return TokenType::XOR;
	else if (in == "GLOBAL") return TokenType::GLOBAL;
	
	else if (in == "T_DOUBLE") return TokenType::T_DOUBLE;
	else if (in == "DOUBLE_128") return TokenType::DOUBLE_128;
	else if (in == "DOUBLE_256") return TokenType::DOUBLE_256;
	
	else if (in == "T_UINT") return TokenType::T_UINT;
	else if (in == "UINT_128") return TokenType::UINT_128;
	else if (in == "UINT_256") return TokenType::UINT_256;
	
	else if (in == "INT_64") return TokenType::INT_64;
	else if (in == "FLOAT_64") return TokenType::FLOAT_64;
	else if (in == "UINT_64") return TokenType::UINT_64;
	
	else if (in == "LOOP") return TokenType::LOOP;
	else if (in == "FOREACH") return TokenType::FOREACH;
	
	else if (in == "STRUCT") return TokenType::STRUCT;
	
	return TokenType::NONE;
}
