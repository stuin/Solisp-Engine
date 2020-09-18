#include "StackRenderer.hpp"

std::vector<StackRenderer> stacks;
int STACKCOUNT;

class Pointer : public Node {
private:
	StackRenderer *mouse;
	StackRenderer *from = NULL;
	StackRenderer *to = NULL;
	sf::RectangleShape rect;

	Solisp::Game *game;
	bool pressed = false;
	bool holding = false;
	double selectionTime = 0;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if(!holding && from != NULL)
			target.draw(rect, states);
	}

public:
	Pointer(StackRenderer *stack, Solisp::Game *game) : Node(POINTER, sf::Vector2i(2, 2)) {
		this->mouse = stack;
		this->game = game;
		collideWith(STACKS);
		stack->setParent(this);
		rect.setSize(sf::Vector2f(150, 174));
		rect.setOutlineColor(sf::Color::Cyan);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineThickness(5);
	}

	void update(double time) {
		if(to != NULL) {
			if(selectionTime > 0)
				selectionTime -= time;
			else {
				to = NULL;
				mouse->setPosition(0, 0);
				mouse->setParent(this);
			}
		}
	}

	void collide(Node *object) override {
		StackRenderer *stack = (StackRenderer *)object;
		if(pressed) {
			if(stack->stack->get_card() != NULL)
				std::cout << stack->stack->get_card()->print_stack() << "\n";
			if(!holding) {
				//Get card count
				int count = 1;
				if(stack->spread) {
					count = stack->checkOffset(getPosition() - stack->getPosition());
					count = stack->stack->get_count() - count;
				}
				std::cout << "Count: " << count << "\n";

				//Pick up cards
				if(game->grab(count, stack->getIndex())) {
					from = stack;
					stack->reload(-1, count);
					mouse->stack = stack->stack;
					mouse->reload(1);
					mouse->setParent(this);
					mouse->setHidden(false);
					holding = true;
				} else
					reloadAll();
			} else {
				//Place down cards
				if(game->place(stack->getIndex()))
					reloadAll();
				else
					from->reload();
				from = NULL;
				holding = false;
				mouse->setHidden(true);
			}
			to = NULL;
			pressed = false;
		} else if(holding && to != stack) {
			//Display possible placement
			to = stack;
			if(game->test(stack->getIndex())) {
				mouse->setParent(to);
				mouse->setPosition(to->getOffset(to->stack->get_count()));
			} else {
				mouse->setPosition(0, 0);
				mouse->setParent(this);
			}
			selectionTime = 2;
		} else if(!holding) {
			from = stack;
			int offset = from->checkOffset(getPosition() - from->getPosition());
			rect.setPosition(from->getPosition() + from->getOffset(offset));
			selectionTime = 2;
		} else if(to == stack)
			selectionTime = 2;
	}

	void recieveEvent(sf::Event event) {
		if(event.mouseButton.button == sf::Mouse::Left)
			pressed = true;
		else if(from != NULL && event.mouseButton.button == sf::Mouse::Right) {
			game->cancel();
			from->reload();
			from = NULL;
			to = NULL;
			mouse->setPosition(0, 0);
			mouse->setParent(this);
			mouse->setHidden(true);
			holding = false;
		}
	}

	void reloadAll() {
		for(int i = 1; i < STACKCOUNT; i++)
			stacks[i].reload();
	}
};