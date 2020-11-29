#include "Skyrmion/UpdateList.h"

using std::function;
typedef function<void(void)> clickptr;
sf::Texture actionTexture;

class ActionButton : public Node {
private:
	clickptr func = NULL;

public:
	ActionButton(int tindex, int y, clickptr func) : Node(MENU, sf::Vector2i(64, 64)) {
		setScale(0.75, 0.75);
		setPosition(40, y);
		setTexture(actionTexture);
		setTextureRect(sf::IntRect(0, tindex * 64, 64, (tindex + 1) * 64));
		this->func = func;

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