#include "card_library.h"

//Base main function
int main(int argc, char const *argv[])
{
	//Build library
	build_library();

	if(argc > 1) {
		std::ifstream infile(argv[1]);
		if(infile.good()) {
			while(!infile.eof())
				std::cout << str_eval(read_stream(infile, STRING)) + "\n";
		}
	} else {
		while(true) {
			std::cout << "test>";
			std::cout << str_eval(read_stream(std::cin, STRING)) + "\n";
		}	
	}
	
}