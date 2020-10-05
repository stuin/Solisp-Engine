#include "Skyrmion/UpdateList.h"
#include "main.h"

#include <X11/Xlib.h>

int main(int argc, char const *argv[]) {
	XInitThreads();

	//Draw background
	sf::RectangleShape rect(sf::Vector2f(1930, 1090));
	rect.setFillColor(sf::Color(7, 99, 36));
	DrawNode background(rect, BACKGROUND);
	UpdateList::addNode(&background);

	buildMenus();

	UpdateList::startEngine("Solitaire", sf::VideoMode(1920, 1080), POINTER);
}