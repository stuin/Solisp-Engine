#include "../Pointer.hpp"

sf::Font font;

class Button : public DrawNode {
private:
	sf::Text text;
	DrawNode *textNode;
	sf::RectangleShape rect;

public:
	Button(string title, int y, Node *parent) : DrawNode(rect, MENU, sf::Vector2i(600, 60), parent) {
		setPosition(350, y);

		//Set up button outline
		rect.setSize(sf::Vector2f(600, 60));
		rect.setFillColor(sf::Color(0, 0, 0, 200));

		//Set up text
		text.setString(title);
		text.setFont(font);
		textNode = new DrawNode(text, TEXT, sf::Vector2i(600,60), this);
		textNode->setPosition(25, 0);

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
			click();
	}

	virtual void click() {}
};

class SubMenu : public Button {
private:
	Node *menu;
	int i;

public:
	SubMenu(int i, string title, Node *menu, Node *parent) : Button(title, (i + 1) * 70, parent) {
		this->menu = menu;
		this->i = i;
	}

	void click();
};