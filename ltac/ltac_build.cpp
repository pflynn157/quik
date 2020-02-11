#include "ltac_build.hh"

//The main build function for the LTAC builder
void LTAC_Builder::build(AstNode *top) {
	for (auto node : top->children) {
		switch (node->type) {
			//Scopes
			case AstType::Scope: {
				var_scope = static_cast<AstScope *>(node);
				build(node); 
			} break;
		
			//Build an extern declaration
			case AstType::ExternFunc: build_extern(node); break;
			
			//Build a regular function declaration
			case AstType::FuncDec: {
				build_func(node);
				build(node);
			} break;
			
			//Build a function call
			case AstType::FuncCall: build_func_call(node); break;
			
			//Build return statements
			case AstType::Return: build_ret(node); break;
			
			//Variables
			case AstType::VarDec: build_var_dec(node); break;
			case AstType::VarAssign: build_var_assign(node); break;
		}
	}
}

#include "ltac.hh"

//Translates an LTAC node to a string
std::string ltac_translate(AsmNode *node) {
	std::string ln = "";
	
	switch (node->type) {
		//Extern labels
		case ltac::Extern: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "extern " + s->val;
		} break;
	
		//Labels
		case ltac::Label: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "lbl " + s->val;
		} break;
		
		//Setup
		case ltac::Setup: ln = "setup\n"; break;
		
		//Leave
		case ltac::Leave: ln = "leave"; break;
		
		//Return
		case ltac::Ret: {
			ln = "ret";
		} break;
		
		//Call
		case ltac::Call: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "call " + s->val;
			ln += "\n";
		} break;
		
		//Move
		case ltac::Mov: {
			ln = "mov ";
		
			AsmNode *arg1 = node->children[0];
			AsmNode *arg2 = node->children[1];
			
			switch (arg1->type) {
				case ltac::Reg: ln += "rx,"; break;
				case ltac::ArgReg: ln += "argx,"; break;
				case ltac::Mem: {
					AsmMem *mem = static_cast<AsmMem *>(arg1);
				
					ln += "[base+";
					ln += std::to_string(mem->index) + "*";
					ln += std::to_string(mem->scale) + "],";
				} break;
				
				//TODO: Add the rest
			}
			
			switch (arg2->type) {
				case ltac::Ptr: ln += " *" + arg2->val; break;
				case ltac::No: ln += " #" + arg2->val; break;
				
				//TODO: Add the rest
			}
		} break;
	}
	
	return ln;
}

