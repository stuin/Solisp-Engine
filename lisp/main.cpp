#include "card_lisp.h"

//Base main function
int main(int argc, char const *argv[])
{
	CardEnviroment env;
	std::cout << "Solisp initialized\n";

	if(argc > 1) {
		std::ifstream infile(argv[1]);
		if(infile.good()) {
			while(!infile.eof())
				std::cout << env.str_eval(env.read_stream(infile, STRING)) + "\n";
		}
	} else {
		while(true) {
			std::cout << "test>";
			std::cout << env.str_eval(env.read_stream(std::cin, STRING)) + "\n";
		}	
	}
	
}