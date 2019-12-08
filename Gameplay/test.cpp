#include "game.h"

#include <iostream>
using std::cout;

int main() {
	cout << "Initializing variables\n";
	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder();

	cout << "Counting slots: ";
	int i = 0;
	while(game.get_stack(i) != NULL)
		i++;
	cout << i << " found\n";

	cout << "Setting up game\n";
	Solisp::Card *card = game.setup(builder);
	card->print_stack();
	game.update();

	cout << " Listing slots\n";
	i = 0;
	while(game.get_stack(i) != NULL && i < 20)	{
		cout << "Stack " << i << ": ";
		if(game.get_stack(i)->get_card() != NULL)
			game.get_stack(i)->get_card()->print_stack();
		cout << "\n";
		i++;
	}
}