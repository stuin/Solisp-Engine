#include "../Skyrmion/UtilNodes.hpp"

sf::Font font;

class StartButton : public ButtonNode {
private:
	sf::Text text;
	DrawNode *textNode;
	sf::RectangleShape rect;
	std::string file;

public:
	StartButton(std::string file, int y, Node *parent) : ButtonNode(NULL, rect, MENU, sf::Vector2i(400, 60), parent) {
		this->file = file;
		setPosition(225, y);
		std::cout << file << "\n";

		//Set up button outline
		rect.setSize(sf::Vector2f(400, 60));
		rect.setFillColor(sf::Color(0, 0, 0, 200));

		//Set up text
		text.setString(file);
		text.setFont(font);
		textNode = new DrawNode(text, TEXT, sf::Vector2i(400,60), this);

		UpdateList::addNode(textNode);
		UpdateList::addNode(this);
	}

	void click() override {
		//Initialize game
		Solisp::Builder builder(file);
		game.setup(&builder);
		game.update();

		//Set up slots
		STACKCOUNT = game.get_stack_count();
		stacks.reserve(STACKCOUNT);
		stacks.emplace_back(game.get_stack(0), 0);
		for(int i = 1; i < STACKCOUNT; i++)
			stacks.emplace_back(game.get_stack(i), i);

		//Set up mouse
		Pointer *mouse = new Pointer(&(stacks[0]), &game);
		UpdateList::setPointer(mouse);
		UpdateList::addNode(mouse);

		//Hide menu
		getParent()->setHidden(true);
	}
};