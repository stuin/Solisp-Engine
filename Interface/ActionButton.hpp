#include "Skyrmion/UpdateList.h"

using std::function;
typedef function<void(void)> clickptr;

class ActionButton : public DrawNode {
private:
	sf::RectangleShape rect;
	clickptr func = NULL;

public:
	ActionButton(clickptr func) : DrawNode(rect, MENU, sf::Vector2i(50, 50)) {
		this->func = func;

		//Set up button outline
		rect.setSize(sf::Vector2f(50, 50));
		rect.setFillColor(sf::Color(0, 0, 0, 200));

		UpdateList::addNode(this);
		UpdateList::addListener(this, sf::Event::MouseButtonPressed);
	}

	void recieveEvent(sf::Event event, int shiftX, int shiftY) {
		sf::Vector2i pos(event.mouseButton.x * shiftX, event.mouseButton.y * shiftY);
		if(event.mouseButton.button == sf::Mouse::Left && getRect().contains(pos))
			if(func != NULL)
				func();
	}
};