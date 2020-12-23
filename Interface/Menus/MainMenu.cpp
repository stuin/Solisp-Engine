#include "FolderMenu.hpp"
#include "menus.h"

#define MENUCOUNT 5
SubMenu *menus[MENUCOUNT];
int openMenu = 0;

void showMenu(int selected, bool toggle) {
	openMenu = selected;
	for(int i = 0; i < MENUCOUNT; i++) {
		if(i != selected && (selected == -1 || menus[i] != menus[selected]->getParent()))
			menus[i]->setHidden(true);
		else if(i == selected && toggle) {
			menus[i]->setHidden(!menus[i]->isHidden());
			if(menus[i]->isHidden())
				openMenu = -1;
		} else
			menus[i]->setHidden(false);
	}
}

clickptr selectMenu(int i) {
	return [i]() {
		//menu->reload();
		showMenu(i, true);
	};
}

int checkOpen() {
	return openMenu;
}

void buildMenus() {
	//Load important resources
	if(!font.loadFromFile("res/RomanAntique.ttf"))
		throw std::invalid_argument("Font file not found");

	//Main menu setup
	menus[0] = new SubMenu(sf::Vector2i(300, 1090));
	menus[0]->addButton("Solitaire", 60, 200, menus[0], selectMenu(2));
	menus[0]->addButton("Load Game", 120, 200, menus[0], selectMenu(3));
	menus[0]->addButton("Themes", 180, 200, menus[0], selectMenu(4));
	menus[0]->setHidden(false);

	//In game menu setup
	menus[1] = new SubMenu(sf::Vector2i(300, 1090));
	menus[1]->addButton("Resume", 60, 200, menus[1], selectMenu(-1));
	menus[1]->addButton("Quit", 120, 200, menus[1], []() { quit(true); });

	//Sub menus
	menus[2] = new FolderMenu("Games", ".solisp", GameNamer, GameFunc, menus[0]);
	menus[3] = new FolderMenu("saves", ".sav", ThemeNamer, LoadFunc, menus[0]);
	menus[4] = new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, menus[0]);
}

int bet(int min, int value, int max) {
	if(value < min)
		return min;
	if(value > max)
		return max;
	return value;
}