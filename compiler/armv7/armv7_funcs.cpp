#include "armv7.hh"

//Builds a function declaration
void Asm_Armv7::build_func_dec(AstNode *node) {
	AstFuncDec *fd = static_cast<AstFuncDec *>(node);
	sec_text.push_back(fd->get_name() + ":");

	sec_text.push_back("push {fp, lr}");
	sec_text.push_back("add fp, sp, #4");
	sec_text.push_back("sub sp, sp, #16");
	sec_text.push_back("");
}

//Builds a function call
void Asm_Armv7::build_func_call(AstNode *node) {
	AstFuncCall *fc = static_cast<AstFuncCall *>(node);
	int r_index = 0;
	
	for (auto child : fc->children) {
		std::string reg = "r" + std::to_string(r_index);
		++r_index;
	
		switch (child->type) {
			//Integers
			case AstType::Int: {
				AstInt *i = static_cast<AstInt *>(child);
				std::string ln = "mov " + reg + ", #";
				ln += std::to_string(i->get_val());
				sec_text.push_back(ln);
			} break;
		
			//Strings
			case AstType::Str: {
				auto name = build_string(child);
				sec_text.push_back("ldr " + reg + ", ." + name);
			} break;
			
			//Other variables
			case AstType::Id: {
				AstID *id = static_cast<AstID *>(child);
				Var v = vars[id->get_name()];
				
				std::string ln = "ldr " + reg;
				ln += ", [fp, #-";
				ln += std::to_string(v.stack_pos) + "]";
				sec_text.push_back(ln);
			} break;
			
			//TODO: Add the rest
		}
	}
	
	sec_text.push_back("bl " + fc->get_name());
	sec_text.push_back("");
}

//Build an extern function call
void Asm_Armv7::build_extern_func(AstNode *node) {
	AstExternFunc *fd = static_cast<AstExternFunc *>(node);
	extern_data.push_back(".extern " + fd->get_name());
}

//Builds a return statement
void Asm_Armv7::build_ret(AstNode *node) {
	if (node->children.size() == 1) {
		auto val = node->children.at(0);
		
		switch (val->type) {
			//Return an integer value
			case AstType::Int: {
				int v = static_cast<AstInt *>(val)->get_val();
				sec_text.push_back("mov r0, #" + std::to_string(v));
			} break;
			
			//Return a variable
			case AstType::Id: {
				auto name = static_cast<AstID *>(val)->get_name();
				Var v = vars[name];
				
				std::string ln = "ldr r0, [fp, #-";
				ln += std::to_string(v.stack_pos) + "]";
				sec_text.push_back(ln);
			} break;
			
			//TODO: Add the reset
		}
		
		sec_text.push_back("");
	}

	sec_text.push_back("sub sp, fp, #4");
	sec_text.push_back("pop {fp, pc}");
	sec_text.push_back("");
	
	stack_pos = 8;
	vars.clear();
}
