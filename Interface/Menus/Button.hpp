#include "../Skyrmion/UpdateList.h"
#include "../main.h" 

sf::Font font;

class Button : public DrawNode {
private:
	sf::Text text;
	DrawNode *textNode;
	sf::RectangleShape rect;
	clickptr func;
	bool locked;

public:
	Button(string title, int y, int width, Node *parent, clickptr func, bool locked = false) : DrawNode(rect, MENU, sf::Vector2i(width, 50), parent) {
		setPosition(width / 8 * 5, y);
		this->func = func;
		this->locked = locked;

		//Set up button outline
		rect.setSize(sf::Vector2f(width, 50));
		rect.setFillColor(sf::Color(1, 17, 6));

		//Set up text
		text.setString(title);
		text.setFont(font);
		textNode = new DrawNode(text, TEXT, sf::Vector2i(width,50), this);
		textNode->setPosition(25, 5);

		//Add to game
		UpdateList::addNode(textNode);
		UpdateList::addNode(this);
	}

	void run() {
		if(func != NULL)
			func();
	}

	void setText(string title) {
		text.setString(title);
	}

	string getText() {
		return text.getString();
	}

	bool isUnlocked() {
		return !locked;
	}
};