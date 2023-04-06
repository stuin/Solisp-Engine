#include "FolderMenu.hpp"
#include "StatText.hpp"
#include "menus.h"
#include "resources.h"

#define MENUCOUNT 7
SubMenu *menus[MENUCOUNT];
Vertex<4> rootVertex(NULL);
int openMenu = 0;

sf::Texture actionTexture;
sf::Font Button::font;

//Show specific menu or sub menu
void showMenu(int selected, bool toggle) {
	openMenu = selected;
	for(int i = 0; i < MENUCOUNT; i++) {
		if(i != selected && (menus[i] != menus[selected]->getParent()))
			menus[i]->setHidden(true);
		else if(i == selected && toggle) {
			menus[i]->setHidden(!menus[i]->isHidden());
			if(menus[i]->isHidden())
				openMenu = MAINMENU;
		} else
			menus[i]->setHidden(false);
	}
}

//Lambda function to open menu
clickptr selectMenu(int i) {
	return [i]() {
		//menus[i]->reload();
		showMenu(i, true);
	};
}

void addActionButton(int tindex, clickptr func) {
	//Create sprite with texture
	sf::Sprite *sprite = new sf::Sprite(actionTexture);
	sprite->setScale(0.75, 0.75);
	sprite->setTextureRect(sf::IntRect(0, tindex * 64, 64, (tindex + 1) * 64));
	menus[ACTIONMENU]->addButton("", func)->setImage(*sprite);
}

void addCardsetButton(string name, const char *start, size_t size) {
	clickptr func = [start, size]() { getCardset()->loadFromMemory((void *)start, size); };
	menus[CARDMENU1]->addButton(name, func, true);
	menus[CARDMENU2]->addButton(name, func, true);
}

int checkOpen() {
	return openMenu;
}

void buildMenus() {
	//Load game resources
	Button::font.loadFromMemory((void *)&_binary_RomanAntique_ttf_start, _RomanAntique_ttf_size);
	actionTexture.loadFromMemory((void *)&_binary_icons_png_start, _icons_png_size);
	//getCardset()->loadFromMemory((void *)&_binary_minimal_dark_png_start, _minimal_dark_png_size);

	//Main menu setup
	menus[MAINMENU] = new SubMenu(sf::Vector2i(250, 1090), 200, &rootVertex);
	Button *startbutton = menus[MAINMENU]->addButton("Solitaire", selectMenu(STARTMENU));
	Button *loadbutton = menus[MAINMENU]->addButton("Load Game", selectMenu(LOADMENU));
	menus[MAINMENU]->addButton("Localhost", []() { joinServer("127.0.0.1"); });
	Button *cardbutton1 = menus[MAINMENU]->addButton("Themes", selectMenu(CARDMENU1));
	menus[MAINMENU]->addButton("Quit", []() { UpdateList::stopEngine(); });
	menus[MAINMENU]->setHidden(false);

	//In game menu setup
	menus[PAUSEMENU] = new SubMenu(sf::Vector2i(250, 1090), 200, &rootVertex);
	Button *actionbutton = menus[PAUSEMENU]->addButton("Resume", selectMenu(ACTIONMENU));
	menus[PAUSEMENU]->addButton("Save & Quit", []() { quitGame(true); });
	menus[PAUSEMENU]->addButton("Abandon Game", []() { quitGame(false); });
	menus[PAUSEMENU]->addButton("Restart Game", []() { restartGame(); });
	Button *cardbutton2 = menus[PAUSEMENU]->addButton("Themes", selectMenu(CARDMENU2));
	new StatText(menus[PAUSEMENU]->addButton("Stats", NULL));

	//Sub menus
	menus[STARTMENU] = new FolderMenu("Games", ".solisp", GameNamer, GameFunc, startbutton, menus[0]);
	menus[LOADMENU] = new FolderMenu("saves", ".sav", ThemeNamer, LoadFunc, loadbutton, menus[0]);
	menus[CARDMENU1] = new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, cardbutton1, menus[0]);
	menus[CARDMENU2] = new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, cardbutton2, menus[1]);

	//Action menu
	menus[ACTIONMENU] = new SubMenu(sf::Vector2i(74, 150), 64, &rootVertex);
	menus[ACTIONMENU]->setPosition(0, -30);
	addActionButton(2, []() { showMenu(PAUSEMENU, false); });

	rootVertex.printAddress();
	startbutton->printAddress();

	//Add system cardsets
	//addCardsetButton("Minimal Dark", &_binary_minimal_dark_png_start, _minimal_dark_png_size);
}