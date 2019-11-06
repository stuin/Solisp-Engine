#include "card_library.h"

//Base main function
int main(int argc, char const *argv[])
{
	//Build library
	build_library();

	if(argc > 1) {
		std::ifstream infile(argv[1]);
		if(infile.good())
			repl("", infile);
	} else
		repl("test>", std::cin);
}