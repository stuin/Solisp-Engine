#include "Root.hpp"
#include "Camera.hpp"

//Static camera functions
Camera *camera = NULL;
void setGameSize(int width, int height) {
	if(camera != NULL)
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
sf::Texture *getCardset() {
	return &cardset;
}

int main(int argc, char const *argv[]) {
	//Draw background
	sf::RectangleShape rect(sf::Vector2f(1930, 1090));
	rect.setFillColor(sf::Color(7, 99, 36));
	DrawNode background(rect, BACKGROUND);
	UpdateList::addNode(&background);

	//Load display resources
	buildMenus();
	root = new Root();
	camera = new Camera();

	//Add undo/redo buttons
	Solisp::Game *gameptr = &game;
	addActionButton(0, [gameptr]() {
		gameptr->undo(2);
		gameptr->update();
		reloadAll();
	});
	/*addActionButton(1, [gameptr]() {
		gameptr->redo(2);
		gameptr->update();
		reloadAll();
	});*/

	UpdateList::startEngine("Solitaire", POINTER);
}

int bet(int min, int value, int max) {
	if(value < min)
		return min;
	if(value > max)
		return max;
	return value;
}