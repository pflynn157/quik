#pragma once

#include <string>
#include <fstream>

#include <ltac/ltac.hh>

class Asm_x64 {
public:
	Asm_x64(LtacFile *f);
	void build_PIC() { pic = true; }
	void write();
protected:
	void build_data(LtacDataSec *data);
	void build_code(LtacCodeSec *code);
	
	void build_func(LtacNode *node);
	void build_func_call(LtacNode *node);
	void build_var(LtacNode *node);
private:
	LtacFile *file;
	bool pic = false;
	std::ofstream writer;
};
