//SFML headers
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

//Game headers
#include "Skyrmion/UpdateList.h"

#include <X11/Xlib.h>

int main() {
	XInitThreads();

	UpdateList::startEngine("Solitaire", sf::VideoMode(1200, 800));
}