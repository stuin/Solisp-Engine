#include "Skyrmion/InputHandler.h"
#include "StackRenderer.hpp"

std::vector<StackRenderer*> stacks;
Solisp::Game game;

std::vector<std::string> selectKeys = {
	"/select/confirm", "/select/back"
};

void reloadAll() {
	for(unc i = 1; i < game.get_stack_count(); i++)
		stacks[i]->reload();
}

class Pointer : public Node {
private:
	//Card selections
	StackRenderer *mouse = NULL;
	StackRenderer *selected = NULL;
	StackRenderer *from = NULL;
	StackRenderer *to = NULL;

	//Interface
	sf::RectangleShape rect;
	DirectionHandler moveInput;
	DirectionHandler selectInput;

	//Game links
	Solisp::GameInterface *gameI = &game;
	unc user = 2;

	//Current state
	bool holding = false;
	double selectionTime = 0;
	int cardOffset = 0;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if(!holding && from != NULL)
			target.draw(rect, states);
	}

public:
	Pointer(Node *root) : Node(POINTER, sf::Vector2i(2, 2), false, root), 
	moveInput("/movement", INPUT, this), selectInput(selectKeys, INPUT, this) {
		collideWith(STACKS);

		//Set up rectangle
		rect.setOutlineColor(sf::Color::Cyan);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineThickness(5);

		//Keyboard movement
		Pointer *_pointer = this;
		moveInput.pressedFunc = [_pointer](int i) {
			StackRenderer *stack = _pointer->selected;
			if(i == _pointer->moveInput.moving && stack != NULL) {
				int offset = _pointer->cardOffset;
				sf::Vector2f pos = stack->getPosition();
				sf::Vector2f size = sf::Vector2f(
					stack->getCardSize().x / 2 + 1, stack->getCardSize().y / 2 + 1);
				
				sf::Vector2f dir = _pointer->moveInput.getMovement(1);
				if(dir.y > 0)
					dir.y *= stack->getCardOffset().y * (stack->stack->height - 1);
				else
					dir.y *= stack->getCardGap().y + 1;
				if(dir.x > 0)
					dir.x *= stack->getCardOffset().x * (stack->stack->width - 1);
				else
					dir.x *= stack->getCardGap().x + 1;

				sf::Vector2f dest = pos;
				if(dir.y > 0 && !_pointer->holding && stack->vspread && 
					offset < stack->stack->get_count())
					dest += stack->getOffset(offset + 1);
				else if(dir.y < 0 && !_pointer->holding && stack->vspread && offset > 0)
					dest += stack->getOffset(offset - 1);
				else
					dest += dir + size;

				if(dest.x > 0 && dest.y > 0) {
					_pointer->setPosition(dest);
					_pointer->cardOffset = -1;
				}
			}
		};

		//Selection input
		selectInput.pressedFunc = [_pointer](int i) {
			if(!_pointer->isHidden()) {
				if(i == 0 && _pointer->selected != NULL)
					_pointer->clickStack(_pointer->selected);
				else if(i == 1 && _pointer->from != NULL)
					_pointer->drop();
			}
		};

		UpdateList::addListener(this, sf::Event::MouseButtonPressed);
		UpdateList::addListener(this, sf::Event::MouseMoved);
		UpdateList::addNode(this);
	}

	void reset(StackRenderer *stack) {
		reset(stack, &game, 2);
	}

	void reset(StackRenderer *stack, Solisp::GameInterface *gameI, unc user) {
		//Default values
		selected = NULL;
		from = NULL;
		to = NULL;
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
		selected = stack;
		if(holding && to != stack) {
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
			if(cardOffset == -1 && stack->spread && stack->stack->get_count() > 0)
				cardOffset = stack->stack->get_count() - 1;
			else
				cardOffset = from->checkOffset(getPosition() - from->getPosition());
			rect.setSize(stack->getCardSize() + sf::Vector2f(1, 1));
			rect.setPosition(from->getPosition() + from->getOffset(cardOffset));
			selectionTime = 2;
		} else if(to == stack)
			selectionTime = 2;
	}

	void clickStack(StackRenderer *stack) {
		if(!holding) {
			//Get card count
			unsigned int count = 1;
			if(stack->spread) {
				count = stack->checkOffset(getPosition() - stack->getPosition());
				count = stack->stack->get_count() - count;
			}

			//Pick up cards
			//cout << count << " cards\n";
			if(gameI->grab(count, stack->getIndex(), user)) {
				from = stack;
				stack->reload(0, count);
				mouse->stack = stack->stack;
				mouse->reload(count);
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
	}

	void recieveEvent(sf::Event event, WindowSize *windowSize) {
		if(checkOpen() != ACTIONMENU) {
			if(holding)
				drop();
			rect.setPosition(-300, 0);
			return;
		}

		if(event.type == sf::Event::MouseButtonPressed && !isHidden()) {
			if(event.mouseButton.button == sf::Mouse::Left && selected != NULL) {
				clickStack(selected);
			} else if(from != NULL && event.mouseButton.button == sf::Mouse::Right)
				drop();
		} else if(event.type == sf::Event::MouseMoved) {
			sf::Vector2f pos = windowSize->worldPos(event.mouseMove.x, event.mouseMove.y);
			setGPosition(pos.x, pos.y);
		}
	}
};