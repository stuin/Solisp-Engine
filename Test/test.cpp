#include "../Gameplay/game.h"

#include <iostream>
using std::cout;

int total = 0;
int failed = 0;

void assert(int value, int expected, string message) {
	if(value != expected) {
		failed++;
		cout << "ERROR: " << value << " != " << expected << ": ";
		cout << message << "\n";
	}
	total++;
}

int main(int argc, char const *argv[]) {
	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder("Games/Klondike.solisp", 10);

	cout << "--Setting up game\n";
	game.setup(builder);

	cout << "--Automated tests\n";

	int counts[15] = {0, 0, 0, 0, 2, 24, 0, 0, 1, 2, 1, 4, 5, 6, 7};
	for(int i = 0; i < 15; i++) {
		assert(game.get_stack(i)->get_count(), counts[i], "Stack has wrong number of cards");
	}

	if(failed == 0)
		cout << "All tests passed\n";
	else
		cout << failed << "/" << total << " tests failed\n";

	cout << "--Listing slots\n";
	int i = 0;
	while(game.get_stack(i) != NULL && i < 30)	{
		cout << "Stack " << i << " at (" <<
			game.get_stack(i)->x << "," << game.get_stack(i)->y << "): ";
		if(game.get_stack(i)->get_card() != NULL)
			cout << game.get_stack(i)->get_card()->print_stack();
		cout << "\n";
		i++;
	}
}