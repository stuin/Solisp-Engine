//SFML headers
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

//Game headers
#include "Skyrmion/UpdateList.h"
#include "../Gameplay/game.h"
#include "../Gameplay/filelist.h"
#include "StackRenderer.hpp"

#include <X11/Xlib.h>

int main(int argc, char const *argv[]) {
	XInitThreads();

	int game_number = 0;
	if(argc > 1) {
		cout << "Game number = " << argv[1] << "\n";
		game_number = argv[1][0] - '0';
	} else {
		cout << "Game number: 0";
		//std::cin >> game_number;
	}

	//Initialize game
	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder(rule_files[game_number]);
	game.setup(builder);
	game.update();

	if(!cardset.loadFromFile("res/base_deck.png"))
		throw std::invalid_argument("Card texture not found");

	//Set up example slot
	std::cout << game.get_stack(14)->get_card()->print_stack() << "\n";
	StackRenderer mouse(game.get_stack(14));
	mouse.setPosition(10, 10);
	UpdateList::addNode(&mouse);

	Node node(1);
	node.setTexture(cardset);
	node.setPosition(300, 10);
	UpdateList::addNode(&node);

	UpdateList::startEngine("Solitaire", sf::VideoMode(1200, 800));
}