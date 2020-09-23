//SFML headers
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

//Engine headers
#include "Skyrmion/UpdateList.h"
#include "../Gameplay/game.h"

#include "Pointer.hpp"
#include "Menus/PlayMenu.hpp"

#include <X11/Xlib.h>

int main(int argc, char const *argv[]) {
	XInitThreads();

	//Load important resources
	if(!cardset.loadFromFile("res/base_deck.png"))
		throw std::invalid_argument("Card texture not found");
	if(!font.loadFromFile("/usr/share/fonts/TTF/DejaVuSerif.ttf"))
		throw std::invalid_argument("Font file not found");

	//Draw background
	sf::RectangleShape rect(sf::Vector2f(1930, 1090));
	rect.setFillColor(sf::Color(7, 99, 36));
	DrawNode background(rect, BACKGROUND);
	UpdateList::addNode(&background);

	//Add menu
	PlayMenu menu;
	UpdateList::addNode(&menu);

	UpdateList::startEngine("Solitaire", sf::VideoMode(1920, 1080), POINTER);
}