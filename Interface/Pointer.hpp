#include "StackRenderer.hpp"

std::vector<StackRenderer> stacks;
int STACKCOUNT;

class Pointer : public Node {
private:
	StackRenderer *mouse;
	StackRenderer *selected = NULL;

	Solisp::Game *game;
	bool pressed = false;

public:
	Pointer(StackRenderer *stack, Solisp::Game *game) : Node(POINTER, sf::Vector2i(16, 16), true) {
		this->mouse = stack;
		this->game = game;
		stack->setParent(this);
		collideWith(STACKS);
	}

	void collide(Node *object) override {
		if(pressed) {
			StackRenderer *stack = (StackRenderer *)object;
			if(selected == NULL) {
				//Pick up cards
				std::cout << stack->getIndex() << "\n";
				if(game->grab(1, stack->getIndex())) {
					selected = stack;
					stack->reload(-1, 1);
					mouse->stack = stack->stack;
					mouse->reload(1);
					mouse->setHidden(false);
				} else
					reloadAll();
			} else {
				//Place down cards
				if(game->place(stack->getIndex()))
					reloadAll();
				else
					selected->reload();
				selected = NULL;
				mouse->setHidden(true);
			}
			pressed = false;
		}
	}

	void recieveEvent(sf::Event event) {
		if(event.mouseButton.button == sf::Mouse::Left)
			pressed = true;
		else if(selected != NULL && event.mouseButton.button == sf::Mouse::Right) {
			game->cancel();
			selected->reload();
			selected = NULL;
			mouse->setHidden(true);
		}
	}

	void reloadAll() {
		for(int i = 1; i < STACKCOUNT; i++)
			stacks[i].reload();
	}
};