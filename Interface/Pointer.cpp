#include "Pointer.h"

std::vector<StackRenderer*> Pointer::stacks;
Solisp::Game Pointer::game;

std::vector<std::string> Pointer::selectKeys = {
	"/select/confirm", "/select/back"
};

sf::Texture StackRenderer::cardset;
float StackRenderer::cardScaling = 0.75;

Pointer::Pointer(Node *root) : Node(POINTER, sf::Vector2i(2, 2), false, root), 
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
			sf::Vector2f pos = stack->getGPosition();
			
			//Get screen size
			Camera *camera = (Camera *)UpdateList::getNode(CAMERA);
			sf::Vector2f corner = camera->getCorner();
			
			//Distance between stacks
			sf::Vector2f dir = _pointer->moveInput.getMovement(1);
			if(dir.y > 0)
				dir.y *= stack->getCardGap().y * stack->stack->height;
			else
				dir.y *= stack->getCardGap().y * 2;
			if(dir.x > 0)
				dir.x *= stack->getCardGap().x * stack->stack->width;
			else
				dir.x *= stack->getCardGap().x * 2;

			//Individual spread cards
			sf::Vector2f dest = pos;
			_pointer->cardOffset = -1;
			if(dir.y != 0 && stack->vspread && !_pointer->holding) {
				if(dir.y > 0 && offset < stack->stack->get_count() - 1) {
					dest += stack->getOffset(offset + 1);
					_pointer->cardOffset = offset;
				} else if(dir.y < 0 && offset > 0 && _pointer->gameI->grab(
						stack->stack->get_count() - offset + 1,
						stack->getIndex(), 1)) {
					dest += stack->getOffset(offset - 1);
					_pointer->cardOffset = offset;
				} else
					dest += dir;
			} else
				dest += dir;

			//Check final position
			if(dest.x > 0 && dest.y > 0 && dest.x < corner.x && dest.y < corner.y)
				_pointer->setGPosition(dest);
			else
				_pointer->cardOffset = offset;
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

void Pointer::reset(StackRenderer *stack) {
	reset(stack, &game, 2);
}

void Pointer::reset(StackRenderer *stack, Solisp::GameInterface *gameI, unc user) {
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

void Pointer::update(double time) {
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

void Pointer::collide(Node *object) {
	StackRenderer *stack = (StackRenderer *)object;
	selected = stack;
	if(holding && to != stack) {
		//Display possible placement
		to = stack;
		if(to == from) {
			//Back to same stack
			mouse->setParent(to);
			mouse->setPosition(to->getOffset(to->stack->get_count()));
		} else if(gameI->test(stack->getIndex(), user)) {
			//On new stack
			mouse->setParent(to);
			mouse->setPosition(to->getOffset(to->stack->get_count() + 1));
		} else {
			//Invalid stack
			mouse->setPosition(10, 10);
			mouse->setParent(this);
		}
		selectionTime = 2;
	} else if(!holding) {
		//Locate selected card
		if(cardOffset == -1 && stack->spread && stack->stack->get_count() > 0) {
			cardOffset = stack->stack->get_count() - 1;
			setPosition(stack->getPosition() + stack->getOffset(cardOffset));
		} else
			cardOffset = stack->checkOffset(getPosition() - stack->getPosition());

		//Highlight selected card
		from = stack;
		rect.setSize(stack->getCardSize() + sf::Vector2f(1, 1));
		rect.setPosition(from->getPosition() + from->getOffset(cardOffset));
		selectionTime = 2;
	} else if(to == stack)
		selectionTime = 2;
}

void Pointer::recieveEvent(sf::Event event, WindowSize *windowSize) {
	if(gameOpen()) {
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

void Pointer::clickStack(StackRenderer *stack) {
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
			UpdateList::sendSignal(RELOADCARDS);

		//Check for win
		if(game.get_remaining() <= 0) {
			quitGame(false);
			setHidden(true);
		}
	} else {
		//Place down cards
		if(gameI->place(stack->getIndex(), user))
			UpdateList::sendSignal(RELOADCARDS);
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

void Pointer::drop() {
	gameI->cancel(user);
	from->reload();
	from = NULL;
	to = NULL;
	mouse->setPosition(0, 0);
	mouse->setParent(this);
	mouse->setHidden(true);
	holding = false;
}

