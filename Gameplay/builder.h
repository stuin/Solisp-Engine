namespace Solisp {
	class Builder;
}

#include "stack.h"

#include <time.h>
#include <vector>
#include <fstream>

using Solisp::Stack;
using std::vector;

/*
 * Created by Stuart Irwin on 4/10/2019.
 * Definitions for reading solisp file
 */

struct layout {
	int x = 1;
	int y = 2;
	unsigned char count = 1;
};

struct setup {
	int seed;
	int width;
	int height;
	unsigned char count;
	std::ifstream *file;
	Solisp::Card *deck;
};

class Solisp::Builder {
private:
	std::ifstream rule_file;

public:
	std::string name;
	int seed;

	Builder(std::string file, unsigned int seed=0) {
		if(seed == 0)
			this->seed = time(NULL);
		else
			this->seed = seed;
		srand(this->seed);

		rule_file.open(file);
		std::getline(rule_file, name);
	}

	~Builder();

	struct setup build_ruleset(Stack *stack);
};
