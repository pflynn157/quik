//x86_utils
//Contains backend-specific stuff
#include "asmx86.hh"

//Converts a node to an assembly type
std::string Asm_x86::type2asm(AstNode *node) {
	std::string ln = "";
	
	switch (node->type) {
		case AstType::Int: {
			AstInt *i = dynamic_cast<AstInt *>(node);
			ln = std::to_string(i->get_val());
		} break;
		
		case AstType::Id: {
			AstID *id = dynamic_cast<AstID *>(node);
			ln = "[" + id->get_name() + "]";
		} break;
		
		case AstType::Char: {
			AstChar *ch = dynamic_cast<AstChar *>(node);
			ln = "\'";
			ln += ch->get_val();
			ln += "\'";
		} break;
		
		case AstType::Bool: {
			AstBool *bl = dynamic_cast<AstBool *>(node);
			if (bl->get_val())
				ln += "1";
			else
				ln += "0";
		} break;
	}
	
	return ln;
}

//Converts a node to floating-point Assembly type
void Asm_x86::type2flt(AstNode *node) {
	switch (node->type) {
		case AstType::Id: {
			AstID *id = dynamic_cast<AstID *>(node);
			sec_text.push_back("fld qword [" + id->get_name() + "]");
		} break;
		
		case AstType::FuncCall: {
			AstFuncCall *fc = dynamic_cast<AstFuncCall *>(node);
			build_func_call(fc);
		} break;
		
		//TODO: Is there a better way we can do this?
		case AstType::Int: {
			AstInt *f = dynamic_cast<AstInt *>(node);
			
			std::string name = "FLT_" + std::to_string(str_index);
			std::string lbl = name + " dq " + std::to_string(f->get_val());
			lbl += ".0";
			++str_index;
			
			sec_data.push_back(lbl);
			sec_text.push_back("fld qword [" + name + "]");
		} break;
		
		case AstType::Float: {
			AstFloat *f = dynamic_cast<AstFloat *>(node);
			
			std::string name = "FLT_" + std::to_string(str_index);
			std::string lbl = name + " dq " + std::to_string(f->get_val());
			++str_index;
			
			sec_data.push_back(lbl);
			sec_text.push_back("fld qword [" + name + "]");
		} break;
	}
}
