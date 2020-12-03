#include "FolderMenu.hpp"
#include "menus.h"

#define MENUCOUNT 3

class MainMenu : public DrawNode {
private:
	Node *menus[MENUCOUNT];
	Button *buttons[MENUCOUNT];
	sf::RectangleShape fade;

public:
	MainMenu() : DrawNode(fade, FADE) {
		//Set up faded background
		fade.setSize(sf::Vector2f(300, 1090));
		fade.setFillColor(sf::Color(0, 0, 0, 50));

		menus[0] = new FolderMenu("Games", ".solisp", GameNamer, GameFunc, this);
		buttons[0] = new Button("Solitaire", 60, 200, this, selectMenu(0));
		menus[1] = new FolderMenu("saves", ".sav", ThemeNamer, LoadFunc, this);
		buttons[1] = new Button("Load Game", 120, 200, this, selectMenu(1));
		menus[2] = new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, this);
		buttons[2] = new Button("Themes", 180, 200, this, selectMenu(2));
	}

	void hideOthers(int selected) {
		for(int i = 0; i < MENUCOUNT; i++) {
			if(i != selected)
				menus[i]->setHidden(true);
		}
	}

	clickptr selectMenu(int i) {
		Node *menu = menus[i];
		MainMenu *parent = this;

		return [menu, i, parent]() {
			if(i == 1)
				((FolderMenu *)menu)->reload();
			menu->setHidden(!menu->isHidden());
			parent->hideOthers(i);
		};
	}
};

MainMenu *menu;

void buildMenus() {
	//Load important resources
	if(!font.loadFromFile("res/RomanAntique.ttf"))
		throw std::invalid_argument("Font file not found");

	menu = new MainMenu();
	UpdateList::addNode(menu);
}

void showMenu() {
	menu->hideOthers(MENUCOUNT + 1);
	menu->setHidden(false);
}

int bet(int min, int value, int max) {
	if(value < min)
		return min;
	if(value > max)
		return max;
	return value;
}