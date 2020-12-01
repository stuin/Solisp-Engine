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

class Solisp::Builder {
private:
	std::ifstream rule_file;
	std::string name;
	int seed;

	sexpr tag_eval(sexpr list, bool layout);

	//Solisp to gameplay convertions
	Card *make_card(const cell &source, bool shuffled);
	Filter *make_filter(const cell &source);

	//Layout interpreting functions
	layout make_slot(Stack &stack, sexpr data, int type, int x, int y);
	layout make_layout(Stack *stack, cell layout, sexpr tags={}, struct layout current={0,0,1});


public:
	Builder(std::string file, unsigned int seed=0) {
		if(seed == 0)
			this->seed = time(NULL);
		else
			this->seed = seed;
		srand(this->seed);

		rule_file.open(file);
		std::getline(rule_file, name);
		std::cout << "Builder initialized with game: " << name << "\n";
	}

	~Builder();

	Card *get_deck();
	int set_stacks(Stack *stack);
	int get_seed() {
		return seed;
	}
};
