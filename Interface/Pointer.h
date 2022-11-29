#include "Skyrmion/InputHandler.h"
#include "StackRenderer.hpp"
#include "Camera.hpp"

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
	static std::vector<StackRenderer*> stacks;
	static Solisp::Game game;
	static std::vector<std::string> selectKeys;

	Pointer(Node *root);

	void reset(StackRenderer *stack);
	void reset(StackRenderer *stack, Solisp::GameInterface *gameI, unc user);

	void update(double time);
	void collide(Node *object) override;
	void recieveEvent(sf::Event event, WindowSize *windowSize);

	void clickStack(StackRenderer *stack);
	void drop();
};