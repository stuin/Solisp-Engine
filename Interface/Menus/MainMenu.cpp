#include "FolderMenu.hpp"
#include "StatText.hpp"
#include "resources.h"

Vertex<4> rootVertex(NULL);
SubMenu *menus[4];

#include "menus.h"

sf::Texture actionTexture;
sf::Font Button::font;

void addActionButton(int tindex, clickptr func) {
	//Create sprite with texture
	sf::Sprite *sprite = new sf::Sprite(actionTexture);
	sprite->setScale(0.75, 0.75);
	sprite->setTextureRect(sf::IntRect(0, tindex * 64, 64, (tindex + 1) * 64));
	menus[1]->addButton("", func)->setImage(*sprite);
}

void addCardsetButton(string name, const char *start, size_t size) {
	clickptr func = [start, size]() { getCardset()->loadFromMemory((void *)start, size); };
	//menus[CARDMENU1]->addButton(name, func, true);
	//menus[CARDMENU2]->addButton(name, func, true);
}

void showMenu(int i) {
	menus[i]->select();
}

bool gameOpen() {
	return !menus[1]->isSelected();
}

void buildMenus() {
	//Load game resources
	Button::font.loadFromMemory((void *)&_binary_RomanAntique_ttf_start, _RomanAntique_ttf_size);
	actionTexture.loadFromMemory((void *)&_binary_icons_png_start, _icons_png_size);
	//getCardset()->loadFromMemory((void *)&_binary_minimal_dark_png_start, _minimal_dark_png_size);

	//Main menu setup
	SubMenu *mainMenu = new SubMenu(sf::Vector2i(250, 1090), 200, &rootVertex);
	menus[0] = mainMenu;
	new FolderMenu("Games", ".solisp", GameNamer, GameFunc, mainMenu->addButton("Solitaire", NULL), mainMenu);
	new FolderMenu("saves", ".sav", ThemeNamer, LoadFunc, mainMenu->addButton("Load Game", NULL), mainMenu);
	mainMenu->addButton("Localhost", []() { joinServer("127.0.0.1"); });
	new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, mainMenu->addButton("Themes", NULL), mainMenu);
	mainMenu->addButton("Quit", []() { UpdateList::stopEngine(); });

	//In game menu setup
	SubMenu *pauseMenu = new SubMenu(sf::Vector2i(250, 1090), 200, &rootVertex);
	Button *resumebutton = pauseMenu->addButton("Resume", NULL);
	pauseMenu->addButton("Save & Quit", []() { quitGame(true); }, mainMenu);
	pauseMenu->addButton("Abandon Game", []() { quitGame(false); }, mainMenu);
	pauseMenu->addButton("Restart Game", []() { restartGame(); });
	new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, pauseMenu->addButton("Themes", NULL), pauseMenu);
	new StatText(pauseMenu->addButton("Stats", NULL));

	//Action menu
	SubMenu *actionMenu = new SubMenu(sf::Vector2i(74, 150), 64, &rootVertex);
	menus[1] = actionMenu;
	actionMenu->setPosition(0, -30);
	addActionButton(2, [resumebutton]() { resumebutton->select(); });
	resumebutton->setVertex(RIGHT, actionMenu);

	//Win menu
	SubMenu *winMenu = new SubMenu(sf::Vector2i(250, 1090), 200, &rootVertex);
	menus[2] = winMenu;
	winMenu->addButton("Main menu", []() { quitGame(false); }, mainMenu);
	winMenu->addButton("Restart Game", []() { restartGame(); });
	new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, winMenu->addButton("Themes", NULL), winMenu);

	mainMenu->select();

	//rootVertex.printAddress();
	//startbutton->printAddress();

	//Add system cardsets
	//addCardsetButton("Minimal Dark", &_binary_minimal_dark_png_start, _minimal_dark_png_size);
}