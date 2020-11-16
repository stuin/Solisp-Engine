#include "Skyrmion/UpdateList.h"
#include "../Gameplay/game.h"
#include "Pointer.hpp"
#include "main.h"

#if __linux__
	#include <X11/Xlib.h>
	#define init XInitThreads();
#else
	#define init void();
#endif

int main(int argc, char const *argv[]) {
	init

	//Draw background
	sf::RectangleShape rect(sf::Vector2f(1930, 1090));
	rect.setFillColor(sf::Color(7, 99, 36));
	DrawNode background(rect, BACKGROUND);
	UpdateList::addNode(&background);

	buildMenus();

	//Load default deck
	if(!cardset.loadFromFile("res/faces/minimal.png"))
		throw std::invalid_argument("Card texture not found");

	UpdateList::startEngine("Solitaire", sf::VideoMode(1920, 1080), POINTER);
}

void startGame(string file) {
	//Initialize game
	Solisp::Builder builder(file);
	game.setup(&builder);
	game.update();

	//Set up slots
	STACKCOUNT = game.get_stack_count();
	stacks.reserve((int)STACKCOUNT);
	stacks.emplace_back(game.get_stack(0), 0);
	for(unc i = 1; i < STACKCOUNT; i++)
		stacks.emplace_back(game.get_stack(i), i);

	//Final setup
	UpdateList::addNode(new Pointer(&(stacks[0]), &game));
}

void changeCardset(string file) {
	if(!cardset.loadFromFile(file))
		throw std::invalid_argument("Card texture not found");
}