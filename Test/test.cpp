#include "../Gameplay/game.h"

#include <iostream>
using std::cout;

int total = 0;
int failed = 0;

void assert(int value, int expected, string message) {
	if(value != expected) {
		failed++;
		cout << "ERROR: " << value << " != " << expected;
		cout << ": Incorrect number of " << message << "\n";
	}
	total++;
}

void assert(bool result, string message) {
	if(!result) {
		failed++;
		cout << "ERROR: " << message << " should be true\n";
	}
	total++;
}

void assert(Stack *value, cardData expected, string message) {
	if(value->get_card() == NULL || !value->get_card()->matches(expected)) {
		failed++;
		cout << "ERROR: " << print_card(value->get_card()->get_data()) << " != " << print_card(expected);
		cout << ": Incorrect card at " << message << "\n";
	}
	total++;
}

void assert_false(bool result, string message) {
	if(result) {
		failed++;
		cout << "ERROR: " << message << " should be false\n";
	}
	total++;
}

int main(int argc, char const *argv[]) {
	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder("Games/Klondike.solisp", 10);
	cout << "Builder initialized with game: " << builder->name << "\n";

	cout << "--Setting up game\n";
	game.setup(builder);

	cout << "--Automated tests\n";

	//Test stack sizes after cards are delt
	int counts[15] = {0, 0, 0, 0, 2, 24, 0, 0, 1, 2, 1, 4, 5, 6, 7};
	for(int i = 0; i < 15; i++) {
		assert(game.get_stack(i)->get_count(), counts[i], "cards in stack");
	}
	assert(game.cards_remaining(), 50, "remaining cards in game");

	//Check test suite
	//assert(1, 2, "Test");
	//assert(false, "Test");
	//assert_false(true, "Test");

	//Try some fake moves
	assert_false(game.place(9, 1), "Fake place 1");
	assert_false(game.grab(2, 11, 1), "Fake grab 1");
	assert(game.grab(1, 13, 1), "Fake grab 2");
	assert_false(game.place(9, 1), "Fake place 2");
	assert(game.grab(1, 9, 1), "Fake grab 3");
	game.cancel(2);
	assert_false(game.place(11, 1), "Fake place 3");

	//Play game
	assert(game.get_stack(4), {2, 'D'}, "stack 4");
	assert(game.grab(1, 9, 1), "Grab 1");
	assert(game.place(11, 1), "Place 1");
	assert(game.get_stack(1), {1, 'C'}, "stack 1");
	assert(game.grab(2, 11, 1), "Grab 2");
	assert(game.place(9, 1), "Place 2");
	assert(game.grab(1, 14, 1), "Grab 3");
	assert(game.place(10, 1), "Place 3");

	//More fake moves
	assert(game.grab(2, 9, 1), "Fake grab 4");
	assert_false(game.place(8, 1), "Fake place 4");
	assert(game.grab(1, 12, 1), "Fake grab 5");
	assert_false(game.place(6, 1), "Fake place 5");
	assert(game.grab(1, 12, 1), "Fake grab 6");
	assert_false(game.place(2, 1), "Fake place 6");
	assert_false(game.grab(1, 4, 1), "Fake grab 7");
	assert_false(game.place(4, 1), "Fake place 7");

	//Continuing game
	assert_false(game.grab(1, 5, 1), "Deal 1");
	assert(game.get_stack(6)->get_count(), 3, "cards after deal 1");
	assert_false(game.grab(1, 5, 1), "Deal 2");
	assert(game.get_stack(7)->get_count(), 3, "cards after deal 2");
	assert(game.grab(1, 6, 1), "Grab 4");
	assert(game.place(13, 1), "Place 4");


	//Report test results
	if(failed == 0)
		cout << "All " << total << " tests passed\n";
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