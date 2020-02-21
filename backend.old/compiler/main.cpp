#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <ast.hh>
#include <build.hh>

//#include <ltac_build.hh>
//#include <ltac.hh>

#include "utils.hh"
#include "types.hh"

#include "armv7/armv7.hh"
#include "x86/asmx86.hh"
//#include "ltac/ltac_gen.hh"

void help() {
	std::cout << "help" << std::endl;
	std::exit(1);
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Fatal: No input specified!" << std::endl;
		std::exit(1);	
	}
	
	//The help option
	if (std::string(argv[1]) == "-h" 
		|| std::string(argv[1]) == "--help") { 
		help();
	}
	
	//Holds options
	Config config;
	
	std::vector<std::string> inputs;
	config.out_name = "out";		// -o
	config.build_lib = false;		// --lib
	config.obj_only = false;		// -c
									// -l
	config.arch = "x86_64";			// -m <i386, x86_64, armv7>
	
	bool asm_only = false;			// -s
	bool optimize = false;			// --optimize
	bool use_ltac = false;			// --ltac
	
	//Iterate through and collect options
	for (int i = 1; i<argc; i++) {
		auto option = std::string(argv[i]);
		
		if (option == "-o") {
			config.out_name = std::string(argv[i+1]);
			i += 1;
		} else if (option == "--lib") {
			config.build_lib = true;
		} else if (option == "-c") {
			config.obj_only = true;
		} else if (option == "-l") {
			config.libs.push_back(std::string(argv[i+1]));
			i += 1;
		} else if (option == "-m") {
			config.arch = std::string(argv[i+1]);
			i += 1;
			
			if (config.arch != "i386" && config.arch != "x86_64"
				&& config.arch != "armv7") {
				std::cout << "Fatal: Unknown architecture." << std::endl;
				return 1;
			}
		} else if (option == "-s") {
			asm_only = true;
		} else if (option == "--optimize") {
			optimize = true;
		} else if (option == "--ltac") {
			use_ltac = true;
			
		//Something else...
		} else {
			if (option[0] == '-') {
				std::cout << "Unknown option: " << option << std::endl;
				std::exit(1);
			} else {
				inputs.push_back(option);
			}
		}
	}
	
	//Check to see if we wish to use the experimental ltac layer
	if (use_ltac) {
		std::cout << "Not currently supported." << std::endl;
		/*std::string path = std::string(inputs[0]);
		auto lines = load_source(path.c_str());
		AstNode *top = build_ast(lines, true, optimize);
		
		LTAC_Builder *builder = new LTAC_Builder("out");
		builder->assemble(top);
		LtacFile *asm_file = builder->get_file();
		
		LTAC_Generator gen;
		gen.build_x86_64(asm_file);
		gen.compile_x86_64("out");
		
		delete builder;
		delete asm_file;
		delete top;*/
	} else {
		//Iterate through each input and work on each file
		//Build for ARM
		if (config.arch == "armv7") {
			std::string path = std::string(inputs[0]);
			Asm_Armv7 builder(config);
			
			auto lines = load_source(path.c_str());
			AstNode *top = build_ast(lines, true, optimize);
			
			builder.assemble(path, top);
			builder.write();
			
			if (!asm_only)
				builder.build();
			
			delete top;
		
		//Build for Intel
		} else {
			Asm_x86 builder(config);
			
			for (auto path : inputs) {
				auto lines = load_source(path.c_str());
				AstNode *node = build_ast(lines, true, optimize);
				
				builder.assemble(path, node);
				builder.write();
				
				delete node;
			}
			
			if (!asm_only)
				builder.build();
		}
	}
	
	return 0;
}