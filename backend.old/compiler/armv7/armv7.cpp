#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

#include "armv7.hh"
#include "../utils.hh"

//The constructor
Asm_Armv7::Asm_Armv7() {
	config.out_name = "out";
}

Asm_Armv7::Asm_Armv7(Config cf) {
	config = cf;
}

//Our recursive assembly function
void Asm_Armv7::assemble(std::string f_path, AstNode *top) {
	if (f_path != "")
		path = f_path;

	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble("", node); break;
			
			case AstType::FuncDec: {
				build_func_dec(node);
				assemble("", node);
			} break;
			
			case AstType::FuncCall: build_func_call(node); break;
			case AstType::ExternFunc: build_extern_func(node); break;
			
			case AstType::VarDec: build_var_dec(node); break;
			case AstType::VarAssign: build_var_assign(node); break;
			
			case AstType::If: {
				std::string lbl = "L" + std::to_string(lbl_index);
				++lbl_index;
				end_lbls.push(lbl);
			}
			case AstType::Elif: 
			case AstType::Else: {
				std::string lbl = "L" + std::to_string(lbl_index);
				++lbl_index;
				labels.push(lbl);
				
				build_conditional(node);
				assemble("", node);
				
				sec_text.push_back("b " + end_lbls.top());
				sec_text.push_back(lbl + ":");
			} break;
			
			case AstType::EndIf: {
				sec_text.push_back(end_lbls.top() + ":");
				end_lbls.pop();
			} break;
			
			case AstType::Return: build_ret(node); break;
		}
	}
}

//Writes everything out into a file
void Asm_Armv7::write() {
	std::string as_path = "/tmp/" + get_basename(path) + ".asm";
	std::ofstream writer(as_path);

	//Write the data section
	writer << ".data" << std::endl;
	
	for (auto ln : sec_data) {
		writer << "\t" << ln << std::endl;
	}
	
	//Write the extern section
	writer << std::endl;
	writer << ".text" << std::endl;
	writer << ".global main" << std::endl;
	
	for (auto ln : extern_data)
		writer << ln << std::endl;
	
	//Write the text section
	writer << std::endl;
	
	for (auto ln : sec_text) {
		if (ln[ln.length()-1] == ':')
			writer << ln << std::endl;
		else
			writer << "\t" << ln << std::endl;
	}
	
	//Write the bottom data section
	writer << std::endl;
	
	for (auto ln : btm_data) {
		writer << ln << std::endl;
	}
	
	//Close everything up...
	writer.close();
}

//Issues the proper command to build the program
//TODO: This needs a lot of work
void Asm_Armv7::build() {
	std::string as_path = "/tmp/" + get_basename(path) + ".asm";
	std::string o_path = "/tmp/" + get_basename(path) + ".o";
	
	std::string out_path = get_path(path) + "out";
	if (config.out_name != "out")
		out_path = config.out_name;
	
	std::string as_cmd = "as " + as_path + " -o " + o_path;
	//std::string ln_cmd = "gcc " + o_path + " -o " + out_path + " -lqkstdlib";
	
	std::string ln_cmd = "ld ";
	ln_cmd += "/usr/lib/arm-linux-gnueabihf/crti.o ";
	ln_cmd += "/usr/lib/arm-linux-gnueabihf/crtn.o ";
	ln_cmd += "/usr/lib/arm-linux-gnueabihf/crt1.o ";
	ln_cmd += "-lc -lqkstdlib ";
	ln_cmd += o_path;
	ln_cmd += " -dynamic-linker /lib/ld-linux.so.3 -o ";
	ln_cmd += out_path;

	system(as_cmd.c_str());
	system(ln_cmd.c_str());
}
