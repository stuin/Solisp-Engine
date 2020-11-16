#include "../Skyrmion/UpdateList.h"
#include "../main.h"

sf::Font font;

using std::function;
typedef function<void(void)> clickptr;

class Button : public DrawNode {
private:
	sf::Text text;
	DrawNode *textNode;
	sf::RectangleShape rect;
	clickptr func = NULL;

public:
	Button(string title, int y, int width, Node *parent, clickptr func) : DrawNode(rect, MENU, sf::Vector2i(width, 50), parent) {
		setPosition(width / 2 + 50, y);
		this->func = func;

		//Set up button outline
		rect.setSize(sf::Vector2f(width, 50));
		rect.setFillColor(sf::Color(0, 0, 0, 200));

		//Set up text
		text.setString(title);
		text.setFont(font);
		textNode = new DrawNode(text, TEXT, sf::Vector2i(width,50), this);
		textNode->setPosition(25, 5);

		//Add to game
		UpdateList::addNode(textNode);
		UpdateList::addNode(this);
		UpdateList::addListener(this, sf::Event::MouseButtonPressed);
	}

	void setText(string title) {
		text.setString(title);
	}

	void recieveEvent(sf::Event event, int shiftX, int shiftY) {
		sf::Vector2i pos(event.mouseButton.x * shiftX, event.mouseButton.y * shiftY);
		if(event.mouseButton.button == sf::Mouse::Left && getRect().contains(pos))
			if(func != NULL)
				func();
	}
};