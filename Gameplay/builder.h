namespace Solisp {
	class Builder;
}

#include "stack.h"

#include <time.h>
#include <vector>

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

	//Solisp to gameplay convertions
	sexpr tag_eval(sexpr list, bool layout);
	Card *make_card(const cell &source, bool shuffled);
	Filter *make_filter(const cell &source);

	//Layout interpreting functions
	layout make_slot(Stack &stack, sexpr data, int type, int x, int y);
	layout make_layout(Stack *stack, cell layout, sexpr tags={}, struct layout current={0,0,1});


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
