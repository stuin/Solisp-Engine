#include "Button.hpp"

bool compareButtons(Button *a, Button *b) {
	return a->getText() < b->getText();
}

class SubMenu : public DrawNode, public Vertex<4> {
private:
	sf::RectangleShape fade;
	std::vector<Button*> contents;
	int buttonWidth, buttonHeight;

public:
	SubMenu(sf::Vector2i size, int buttonWidth, Vertex<4> *_root, Node *parent=NULL) 
	: DrawNode(fade, FADE, size, parent), Vertex<4>(_root) {
		//Set up faded background
		fade.setSize(sf::Vector2f(size.x, size.y));
		fade.setFillColor(sf::Color(0, 0, 0, 50));
		this->buttonWidth = buttonWidth;
		buttonHeight = 60;

		_root->setVertex(RIGHT, this, LEFT);

		setOrigin(0, 0);
		setHidden(true);
		UpdateList::addNode(this);
		//UpdateList::addListener(this, sf::Event::MouseMoved);
		UpdateList::addListener(this, sf::Event::MouseButtonPressed);
	}

	void recieveEvent(sf::Event event, WindowSize *windowSize) {
		if(event.type == sf::Event::MouseButtonPressed) {
			sf::Vector2f pos = windowSize->worldPos(
				event.mouseButton.x, event.mouseButton.y);
			if(!isHidden() && event.mouseButton.button == sf::Mouse::Left && 
				getRect().contains(pos)) {
				for(Button *b : contents) {
					if(b->getRect().contains(pos))
						b->run();
				}
			}
		}
	}

	Button *addButton(string title, clickptr func, bool locked=false) {
		Button *b = new Button(title, buttonHeight * (contents.size() + 1), 
			buttonWidth, this, getRoot(), func, locked);
		int i = contents.size();
		contents.push_back(b);

		if(i == 0) {
			getRoot()->setVertex(RIGHT, contents[i], LEFT);
			setVertex(DOWN, contents[i], UP);
		} else {
			contents[0]->addVertex(DOWN, contents[i], UP);
			contents[i]->setVertex(LEFT, getRoot());
		}
		return contents[i];
	}

	void onSelect(bool selected) {
		//if(contents.size() > 0)
		//	contents[0]->select();
		setHidden(!selected);
		Node *parent = getParent();
		while(parent != NULL) {
			parent->setHidden(!selected);
			parent = parent->getParent();
		}
	}

	void clearContents() {
		getRoot()->setVertex(RIGHT, this);
		int i = 0;
		for(auto it = contents.begin(); it != contents.end(); ++it) {
			if((*it)->isUnlocked()) {
				(*it)->setDelete();
				contents.erase(it);
			} else {
				(*it)->setPosition((*it)->getPosition().x, buttonHeight * ++i);
			}
		}
	}

	void sortContents() {
		sort(contents.begin(), contents.end(), compareButtons);

		for(int i = 0; i < (int)contents.size(); i++) {
			Button *b = contents[i];
			if(i == 0) {
				getRoot()->setVertex(RIGHT, b);
				setVertex(DOWN, contents[i]);
			} else
				b->setVertex(UP, contents[i-1]);
			b->setPosition(b->getPosition().x, buttonHeight * (i + 1));
		}
	}

	virtual void reload() {

	}
};