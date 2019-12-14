#include <iostream>
#include <string>
#include <fstream>

#include <lex.hh>
#include <ast.hh>

#include "build.hh"
#include "parse.hh"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		Line l;
		l.no = 1;
		
		std::string ln = "";
		std::cout << "$> ";
		std::getline(std::cin, ln);
		
		l.original = ln;
		l.tokens = tokenize(ln);
		
		AstNode *node = build_node(l);
		
		std::cout << std::endl << "AST: " << std::endl;
		print_tree(node);
	} else {
		auto lines = load_source(argv[1]);
		AstNode *top = build_ast(lines);
		print_tree(top);
	}
	
	return 0;
}