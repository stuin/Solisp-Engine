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
		rect.setFillColor(sf::Color(1, 17, 6));

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

class SubMenu : public DrawNode {
private:
	sf::RectangleShape fade;
	std::vector<Button*> contents;

public:
	SubMenu(sf::Vector2i size, Node *parent=NULL) : DrawNode(fade, FADE, size, parent) {
		//Set up faded background
		fade.setSize(sf::Vector2f(size.x, size.y));
		fade.setFillColor(sf::Color(0, 0, 0, 50));

		setOrigin(0, 0);
		setHidden(true);
		UpdateList::addNode(this);
	}

	void addButton(string title, int y, int width, Node *parent, clickptr func) {
		contents.push_back(new Button(title, y, width, parent, func));
	}

	void clearContents() {
		for(Button *b : contents)
			b->setDelete();
		contents.clear();
	}

	virtual void reload() {

	}
};