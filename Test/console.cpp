#include "../Gameplay/game.h"
#include "../Gameplay/filelist.h"

#include <iostream>
using std::cout;

int main(int argc, char const *argv[]) {
	int game_number;
	if(argc > 1) {
		cout << "Game number = " << argv[1] << "\n";
		game_number = argv[1][0] - '0';
	} else {
		cout << "Game number: ";
		std::cin >> game_number;
	}

	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder(rule_files[game_number]);

	cout << "--Setting up game\n";
	game.setup(builder);

	cout << "--Listing slots\n";
	int i = 0;
	while(game.get_stack(i) != NULL && i < game.get_stack_count())	{
		cout << "Stack " << i << " at (" <<
			game.get_stack(i)->x << "," << game.get_stack(i)->y << "): ";
		if(game.get_stack(i)->get_card() != NULL)
			cout << game.get_stack(i)->get_card()->print_stack();
		cout << "\n";
		i++;
	}
}