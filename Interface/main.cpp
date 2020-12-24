#include "Skyrmion/UpdateList.h"
#include "../Gameplay/game.h"
#include "Pointer.hpp"
#include "ActionButton.hpp"
#include "main.h"

StackRenderer *themeView = NULL;
Pointer *pointer = NULL;
string next_save_file;

//Side buttons
ActionButton *menuButton = NULL;
ActionButton *undoButton = NULL;
ActionButton *redoButton = NULL;

int main(int argc, char const *argv[]) {
	//Draw background
	sf::RectangleShape rect(sf::Vector2f(1930, 1090));
	rect.setFillColor(sf::Color(7, 99, 36));
	DrawNode background(rect, BACKGROUND);
	UpdateList::addNode(&background);

	//Load defaults
	buildMenus();
	changeCardset("res/faces/minimal_dark.png");
	if(!actionTexture.loadFromFile("res/icons.png"))
		throw std::invalid_argument("Button textures not found");

	//Add decorational Cards
	Solisp::Stack *stack = new Solisp::Stack();
	stack->set_max(5);
	stack->add_count(5);
	stack->set_tag(SPREAD);
	stack->set_tag(SPREAD_HORIZONTAL);
	stack->set_card(new Solisp::Card({1,'S', 4,'D', 7,'C', 12,'H', 1,'J'}, 8));
	themeView = new StackRenderer(stack, 1, DISPLAY, 1.3);
	themeView->setPosition(700, 400);

	//Add quit button
	Solisp::Game *gameptr = &game;
	menuButton = new ActionButton(2, 40, []() {
		if(checkOpen() == -1)
			showMenu(1);
	});

	//Add undo/redo buttons
	undoButton = new ActionButton(0, 100, [gameptr]() {
		if(checkOpen() == -1) {
			gameptr->undo(2);
			gameptr->update();
			reloadAll();
		}
	});
	redoButton = new ActionButton(1, 160, [gameptr]() {
		if(checkOpen() == -1) {
			gameptr->redo(2);
			gameptr->update();
			reloadAll();
		}
	});

	UpdateList::startEngine("Solitaire", POINTER);
}

void startGame(string rule_file, string save_file) {
	if(save_file == "") {
		//Initialize game
		Solisp::Builder builder(rule_file);
		game.setup(&builder);
		game.update();
	} else {
		game.load(save_file, rule_file);
		std::remove(save_file.c_str());
	}

	//Get next save file name
	int startI = rule_file.find_last_of(slash);
	next_save_file = "saves/";
	next_save_file += rule_file.substr(startI, rule_file.length() - startI - 6);
	int n = 1;
	while(std::filesystem::exists(next_save_file + std::to_string(n) + ".sav"))
		++n;
	next_save_file += std::to_string(n) + ".sav";

	//Set up slots
	STACKCOUNT = game.get_stack_count();
	stacks.clear();
	stacks.reserve((int)STACKCOUNT);
	stacks.push_back(new StackRenderer(game.get_stack(0), 0, POINTER));
	for(unc i = 1; i < STACKCOUNT; i++)
		stacks.push_back(new StackRenderer(game.get_stack(i), i));

	//Final setup
	menuButton->setHidden(false);
	undoButton->setHidden(false);
	themeView->setHidden(true);

	if(pointer == NULL) {
		pointer = new Pointer(stacks[0]);
		UpdateList::addNode(pointer);
	} else
		pointer->reset(stacks[0]);
}

void changeCardset(string file) {
	if(!cardset.loadFromFile(file))
		throw std::invalid_argument("Card texture not found");
}

void quit(bool save) {
	if(save)
		game.save(next_save_file);

	//Reset menu buttons
	menuButton->setHidden(true);
	undoButton->setHidden(true);
	redoButton->setHidden(true);

	for(StackRenderer *s : stacks)
		s->setDelete();
	game.clear();

	//Show menu
	pointer->setHidden(true);
	themeView->setHidden(false);
	showMenu(0);
}