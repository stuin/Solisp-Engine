#include "Skyrmion/UpdateList.h"
#include "../Gameplay/game.h"
#include "Pointer.hpp"
#include "main.h"

StackRenderer *themeView;

#if __linux__
	#include <X11/Xlib.h>
	#define init XInitThreads
#else
	#define init void
#endif

int main(int argc, char const *argv[]) {
	init();

	//Draw background
	sf::RectangleShape rect(sf::Vector2f(1930, 1090));
	rect.setFillColor(sf::Color(7, 99, 36));
	DrawNode background(rect, BACKGROUND);
	UpdateList::addNode(&background);

	//Load defaults
	buildMenus();
	changeCardset("res/faces/minimal.png");

	//Add decorational Cards
	Solisp::Stack *stack = new Solisp::Stack();
	stack->set_max(5);
	stack->add_count(5);
	stack->set_tag(SPREAD);
	stack->set_tag(SPREAD_HORIZONTAL);
	stack->set_card(new Solisp::Card({1,'S', 4,'D', 7,'C', 12,'H', 1,'J'}, 8));
	themeView = new StackRenderer(stack, 1, 1.3);
	themeView->setPosition(700, 400);

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
	themeView->setHidden(true);
	UpdateList::addNode(new Pointer(&(stacks[0]), &game));
}

void changeCardset(string file) {
	if(!cardset.loadFromFile(file))
		throw std::invalid_argument("Card texture not found");
}

int bet(int min, int value, int max) {
	if(value < min)
		return min;
	if(value > max)
		return max;
	return value;
}