#include "Root.hpp"
#include "Camera.hpp"
#include "resources.h"

//Static camera functions
Camera *camera = NULL;
void setGameSize(int width, int height) {
	camera->setGameSize(width, height);
}

//Static Root functions
Root *root = NULL;
void startGame(string rule_file, string save_file) {
	root->startGame(rule_file, save_file);
}
void quitGame(bool save) {
	root->quitGame(save);
}

void changeCardset(string file) {
	if(!cardset.loadFromFile(file))
		throw std::invalid_argument("Card texture not found");
}

int main(int argc, char const *argv[]) {
	//Draw background
	sf::RectangleShape rect(sf::Vector2f(1930, 1090));
	rect.setFillColor(sf::Color(7, 99, 36));
	DrawNode background(rect, BACKGROUND);
	UpdateList::addNode(&background);

	//Load menu resources
	sf::Font font;
	if(!font.loadFromMemory((void *)&_binary_RomanAntique_ttf_start, _RomanAntique_ttf_size))
		throw std::invalid_argument("Font file not found");
	buildMenus(font);

	//Load display defaults
	camera = new Camera();
	changeCardset("res/faces/minimal_dark.png");
	if(!actionTexture.loadFromMemory((void *)&_binary_icons_png_start, _icons_png_size))
		throw std::invalid_argument("Button textures not found");

	root = new Root();
	UpdateList::startEngine("Solitaire", POINTER);
}