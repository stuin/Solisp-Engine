#include "StackRenderer.hpp"

std::vector<StackRenderer*> stacks;
Solisp::Game game;

void reloadAll() {
	for(unc i = 1; i < game.get_stack_count(); i++)
		stacks[i]->reload();
}

class Pointer : public Node {
private:
	//Interface
	StackRenderer *mouse = NULL;
	StackRenderer *from = NULL;
	StackRenderer *to = NULL;
	sf::RectangleShape rect;

	//Game links
	Solisp::GameInterface *gameI = &game;
	unc user = 2;

	//Current state
	bool pressed = false;
	bool holding = false;
	double selectionTime = 0;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if(!holding && from != NULL)
			target.draw(rect, states);
	}

public:
	Pointer(Node *root) : Node(POINTER, sf::Vector2i(2, 2), false, root) {
		collideWith(STACKS);

		//Set up rectangle
		rect.setOutlineColor(sf::Color::Cyan);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineThickness(5);

		UpdateList::addListener(this, sf::Event::MouseButtonPressed);
		UpdateList::addListener(this, sf::Event::MouseMoved);
		UpdateList::addNode(this);
	}

	void reset(StackRenderer *stack) {
		reset(stack, &game, 2);
	}

	void reset(StackRenderer *stack, Solisp::GameInterface *gameI, unc user) {
		//Default values
		from = NULL;
		to = NULL;
		pressed = false;
		holding = false;

		//External references
		this->user = user;
		this->gameI = gameI;
		this->mouse = stack;

		stack->setParent(this);
		setHidden(false);
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
		if(pressed && !isHidden() && game.get_stage() == PLAYING) {
			//if(stack->stack->get_card() != NULL)
			//	std::cout << stack->stack->get_card()->print_stack() << "\n";
			if(!holding) {
				//Get card count
				unsigned int count = 1;
				if(stack->spread) {
					count = stack->checkOffset(getPosition() - stack->getPosition());
					count = stack->stack->get_count() - count;
				}

				//Pick up cards
				if(gameI->grab(count, stack->getIndex(), user)) {
					from = stack;
					stack->reload(0, count);
					mouse->stack = stack->stack;
					mouse->reload(1);
					mouse->setParent(this);
					mouse->setHidden(false);
					holding = true;
				} else
					reloadAll();

				//Check for win
				if(game.get_remaining() <= 0) {
					quitGame(false);
					setHidden(true);
				}
			} else {
				//Place down cards
				if(gameI->place(stack->getIndex(), user))
					reloadAll();
				else
					from->reload();
				from = NULL;
				holding = false;
				mouse->setHidden(true);

				//Check for win
				if(game.get_remaining() <= 0) {
					quitGame(false);
					setHidden(true);
				}

			}
			to = NULL;
			pressed = false;
		} else if(holding && to != stack) {
			//Display possible placement
			to = stack;
			if(to == from) {
				mouse->setParent(to);
				mouse->setPosition(to->getOffset(to->stack->get_count()));
			} else if(gameI->test(stack->getIndex(), user)) {
				mouse->setParent(to);
				mouse->setPosition(to->getOffset(to->stack->get_count() + 1));
			} else {
				mouse->setPosition(0, 0);
				mouse->setParent(this);
			}
			selectionTime = 2;
		} else if(!holding) {
			from = stack;
			int offset = from->checkOffset(getPosition() - from->getPosition());
			rect.setSize(stack->getCardSize() + sf::Vector2f(1, 1));
			rect.setPosition(from->getPosition() + from->getOffset(offset));
			selectionTime = 2;
		} else if(to == stack)
			selectionTime = 2;

		if(pressed)
			pressed = false;
	}

	void drop() {
		gameI->cancel(user);
		from->reload();
		from = NULL;
		to = NULL;
		mouse->setPosition(0, 0);
		mouse->setParent(this);
		mouse->setHidden(true);
		holding = false;
		pressed = false;
	}

	void recieveEvent(sf::Event event, int shiftX, int shiftY) {
		if(checkOpen() != ACTIONMENU) {
			if(holding)
				drop();
			rect.setPosition(-300, 0);
			return;
		}

		if(event.type == sf::Event::MouseButtonPressed && !isHidden()) {
			if(event.mouseButton.button == sf::Mouse::Left)
				pressed = true;
			else if(from != NULL && event.mouseButton.button == sf::Mouse::Right)
				drop();
		} else if(event.type == sf::Event::MouseMoved) {
			setGPosition(event.mouseMove.x * shiftX, event.mouseMove.y * shiftY);
		} else if(event.type == sf::Event::MouseButtonReleased)
			pressed = false;
	}
};