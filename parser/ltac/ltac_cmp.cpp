#include "ltac_build.hh"
#include "ltac.hh"

#include <types.hh>
#include <ast.hh>
#include <parse.hh>

//Returns the datatype of an ltac node
DataType LTAC_Builder::determine_type(LtacNode *node) {
	auto type = DataType::None;
	
	switch (node->type) {
		case ltac::Int: type = DataType::Int; break;
		case ltac::Float: type = DataType::Float; break;
		case ltac::String: type = DataType::Str; break;
		
		case ltac::Var: {
			auto var = static_cast<LtacVar *>(node);
			type = var->d_type;
		} break;
	}
	
	return type;
}

void LTAC_Builder::build_cmp(AstNode *node) {
	auto cmp = static_cast<AstCond *>(node);
	auto lcmp = new LtacCmp;
	
	auto lval = convert_ast_var(cmp->lval);
	auto rval = convert_ast_var(cmp->rval);
	
	//Check to make sure right/left value types match
	//The left value determines this
	auto l_type = determine_type(lval);
	auto r_type = determine_type(rval);
	
	if (l_type != r_type) {
		Line ln;
		syntax_error(ln, "You can only compare values of the same type.");
	}
	
	//Set the comparison type
	if (l_type == DataType::Int)
		lcmp = new LtacICmp;
		
	//Set everything
	lcmp->lval = lval;
	lcmp->rval = rval;
	
	switch (cmp->get_op()) {
		case CondOp::Equals: lcmp->op = Operator::NotEqual; break;
		case CondOp::NotEquals: lcmp->op = Operator::Equal; break;
		case CondOp::Greater: lcmp->op = Operator::LessEq; break;
		case CondOp::GreaterEq: lcmp->op = Operator::LessEq; break;
		case CondOp::Less: lcmp->op = Operator::GreaterEq; break;
		case CondOp::LessEq: lcmp->op = Operator::GreaterEq; break;
	}
	
	//Add the comparison
	file->code->children.push_back(lcmp);
}