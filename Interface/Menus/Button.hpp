#include "../Skyrmion/UpdateList.h"
#include "../Skyrmion/VertexGraph.hpp"
#include "../main.h" 

class Button : public DrawNode, public Vertex<4> {
private:
	sf::Text text;
	DrawNode *textNode;
	sf::RectangleShape rect;
	clickptr func;
	bool locked;

public:
	static sf::Font font;

	Button(string title, int y, int width, Node *parent, Vertex<4> *root, clickptr func, bool locked = false) 
	: DrawNode(rect, MENU, sf::Vector2i(width, 50), parent), Vertex(root) {
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
		printAddress();
		select(RIGHT)->printAddress();
		if(func != NULL)
			func();
	}

	void onSelect(bool selected) {
		Node *parent = getParent();
		while(parent != NULL) {
			parent->setHidden(!selected);
			parent = parent->getParent();
		}
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