#include "FolderMenu.hpp"
#include "StatText.hpp"
#include "resources.h"

Vertex<4> rootVertex(NULL);
Vertex<4> *actionVertex;

#include "menus.h"

sf::Texture actionTexture;
sf::Font Button::font;

void addActionButton(int tindex, clickptr func) {
	//Create sprite with texture
	sf::Sprite *sprite = new sf::Sprite(actionTexture);
	sprite->setScale(0.75, 0.75);
	sprite->setTextureRect(sf::IntRect(0, tindex * 64, 64, (tindex + 1) * 64));
	((SubMenu*)actionVertex)->addButton("", func)->setImage(*sprite);
}

void addCardsetButton(string name, const char *start, size_t size) {
	clickptr func = [start, size]() { getCardset()->loadFromMemory((void *)start, size); };
	//menus[CARDMENU1]->addButton(name, func, true);
	//menus[CARDMENU2]->addButton(name, func, true);
}

bool gameOpen() {
	return actionVertex->isSelected();
}

void buildMenus() {
	//Load game resources
	Button::font.loadFromMemory((void *)&_binary_RomanAntique_ttf_start, _RomanAntique_ttf_size);
	actionTexture.loadFromMemory((void *)&_binary_icons_png_start, _icons_png_size);
	//getCardset()->loadFromMemory((void *)&_binary_minimal_dark_png_start, _minimal_dark_png_size);

	//Main menu setup
	SubMenu *main = new SubMenu(sf::Vector2i(250, 1090), 200, &rootVertex);
	Button *startbutton = main->addButton("Solitaire", NULL);
	Button *loadbutton = main->addButton("Load Game", NULL);
	main->addButton("Localhost", []() { joinServer("127.0.0.1"); });
	Button *cardbutton1 = main->addButton("Themes", NULL);
	main->addButton("Quit", []() { UpdateList::stopEngine(); });
	main->setHidden(false);

	//In game menu setup
	SubMenu *pause = new SubMenu(sf::Vector2i(250, 1090), 200, &rootVertex);
	Button *resumebutton = pause->addButton("Resume", NULL);
	pause->addButton("Save & Quit", []() { quitGame(true); });
	pause->addButton("Abandon Game", []() { quitGame(false); });
	pause->addButton("Restart Game", []() { restartGame(); });
	Button *cardbutton2 = pause->addButton("Themes", NULL);
	new StatText(pause->addButton("Stats", NULL));

	//File based menus
	new FolderMenu("Games", ".solisp", GameNamer, GameFunc, startbutton, main);
	new FolderMenu("saves", ".sav", ThemeNamer, LoadFunc, loadbutton, main);
	new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, cardbutton1, main);
	new FolderMenu("res/faces", ".png", ThemeNamer, ThemeFunc, cardbutton2, pause);

	//Action menu
	SubMenu *actions = new SubMenu(sf::Vector2i(74, 150), 64, &rootVertex);
	actions->setPosition(0, -30);
	actionVertex = actions;
	addActionButton(2, [resumebutton]() { resumebutton->select(); });
	resumebutton->setVertex(RIGHT, actionVertex);

	rootVertex.printAddress();
	startbutton->printAddress();

	//Add system cardsets
	//addCardsetButton("Minimal Dark", &_binary_minimal_dark_png_start, _minimal_dark_png_size);
}