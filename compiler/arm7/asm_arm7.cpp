#include "asm_arm7.hh"

//Build the data section
void Asm_Arm7::build_data(LtacDataSec *data) {
	for (auto node : data->children) {
		switch (node->type) {
			case ltac::String: {
				auto str = static_cast<LtacString *>(node);
				std::string d_name = ".d_" + str->name;
				
				writer << d_name << ": .ascii \"" << str->val;
				writer << "\\000\"" << std::endl;
				
				std::string ln = str->name + ": .word " + d_name;
				str_labels.push_back(ln);
			} break;
			
			//TODO: Add the rest
		}
	}
}

//Build the code section
void Asm_Arm7::build_code(LtacCodeSec *code) {
	for (auto ln : code->children) {
		switch (ln->type) {
			case ltac::Func: build_func(ln); break;
			case ltac::FuncCall: build_func_call(ln); break;
			case ltac::Ret: build_ret(ln); break;
		}
	}
	
	//Write out the data labels
	for (auto ln : str_labels)
		writer << ln << std::endl;
	writer << std::endl;
}

//Builds a function declaration
void Asm_Arm7::build_func(LtacNode *node) {
	auto fd = static_cast<LtacFunc *>(node);
	
	if (fd->name == "main") {
		writer << ".global main" << std::endl;
	}
	
	writer << std::endl;
	writer << fd->name << ":" << std::endl;
	
	writer << "\tpush {fp, lr}" << std::endl;
	writer << "\tadd fp, sp, #4" << std::endl;
	writer << std::endl;
}

//Builds function calls
void Asm_Arm7::build_func_call(LtacNode *node) {
	auto fc = static_cast<LtacFuncCall *>(node);
	int call_index = 0;
	
	//Build the arguments
	for (auto arg : fc->children) {
		switch (arg->type) {
			//Strings
			case ltac::String: {
				auto str = static_cast<LtacString *>(arg);
				writer << "\tldr r" << std::to_string(call_index);
				writer << ", " << str->name << std::endl;
				++call_index;
			} break;
			
			//TODO: Add the rest
		}
	}
	
	//Finally, build the call
	writer << "\tbl " << fc->name << std::endl;
	writer << std::endl;
}

//Builds a function return
void Asm_Arm7::build_ret(LtacNode *node) {
	writer << "\tmov r0, #0" << std::endl;
	writer << "\tpop {fp, pc}" << std::endl;
	writer << std::endl;
}

