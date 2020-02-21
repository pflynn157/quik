#pragma once

#include <string>

#include <ltac.hh>

class LTAC_Generator {
public:
	LTAC_Generator(){}
	
	void build_x86_64(LtacFile *file);
	void compile_x86_64(std::string bin);
protected:
	std::string build_operand(LtacNode *node);
private:
	std::string output;
	int stack_base = 4;
};