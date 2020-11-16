#include "Skyrmion/UpdateList.h"
#include "main.h"

#if __linux__
	#include <X11/Xlib.h>
	#define init XInitThreads();
#else
	#define init
#endif

int main(int argc, char const *argv[]) {
	init

	//Draw background
	sf::RectangleShape rect(sf::Vector2f(1930, 1090));
	rect.setFillColor(sf::Color(7, 99, 36));
	DrawNode background(rect, BACKGROUND);
	UpdateList::addNode(&background);

	buildMenus();

	UpdateList::startEngine("Solitaire", sf::VideoMode(1920, 1080), POINTER);
}