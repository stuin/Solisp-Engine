#include "Root.hpp"

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
void joinServer(string ip) {
	root->joinServer(ip);
}
void quitGame(bool save) {
	root->quitGame(save);
}
void restartGame() {
	root->restartGame();
}
sf::Texture *getCardset() {
	return &StackRenderer::cardset;
}

int main(int argc, char const *argv[]) {
	Settings::loadSettings("res/settings.json");

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
	Solisp::Game *gameptr = &Pointer::game;
	addActionButton(0, [gameptr]() {
		gameptr->undo(2, true);
		UpdateList::sendSignal(RELOADCARDS);
	});
	/*addActionButton(1, [gameptr]() {
		gameptr->redo(2);
		gameptr->update();
		reloadAll();
	});*/

	//Efficiency settings
	UpdateList::pauseLayer(BACKGROUND);
	UpdateList::pauseLayer(MENU);
	UpdateList::pauseLayer(TEXT);
	UpdateList::staticLayer(STACKS);
	UpdateList::staticLayer(DISPLAY);
	UpdateList::staticLayer(ACTIONS);
	UpdateList::staticLayer(FADE);
	UpdateList::staticLayer(INPUT);
	UpdateList::staticLayer(POINTER);
	UpdateList::staticLayer(CAMERA);

	UpdateList::startEngine("Solitaire", CAMERA);
}

int bet(int min, int value, int max) {
	if(value < min)
		return min;
	if(value > max)
		return max;
	return value;
}