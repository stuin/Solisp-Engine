#include "game.h"
#include "../Godot/filelist.h"

#include <iostream>
using std::cout;

int main() {
	int game_number = 2;
	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder(rule_files[game_number]);

	cout << "Setting up game\n";
	game.setup(builder);
	game.update();

	cout << "\nListing slots\n";
	int i = 0;
	while(game.get_stack(i) != NULL && i < 30)	{
		cout << "Stack " << i << " at (" << 
			game.get_stack(i)->x << "," << game.get_stack(i)->y << "): ";
		if(game.get_stack(i)->get_card() != NULL)
			game.get_stack(i)->get_card()->print_stack();
		cout << "\n";
		i++;
	}
}