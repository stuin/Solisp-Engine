#include "Skyrmion/Node.h"

class Camera : public Node {
	sf::RectangleShape gameSize;

public:
	Camera() : Node(POINTER, sf::Vector2i(1920, 1080)) {
		setPosition(1920 / 2, 1080 / 2);

		//Setup game outline
		gameSize.setOutlineColor(sf::Color::Green);
		gameSize.setFillColor(sf::Color::Transparent);
		gameSize.setOutlineThickness(5);
		gameSize.setPosition(100, 30);
		UpdateList::addNode(new DrawNode(gameSize, DISPLAY));

		UpdateList::addNode(this);
		UpdateList::addListener(this, sf::Event::Resized);
	}

	void recieveEvent(sf::Event event, int shiftX, int shiftY) {
		std::cout << event.size.width << ", " << event.size.height << "\n";
		sf::Vector2f size(event.size.width, event.size.height);

		setPosition(size.x / 2, size.y / 2);
		UpdateList::setCamera(this, sf::Vector2f(size.x, size.y));
	}

	void setGameSize(int width, int height) {
		if(width == -1 && height == -1)
			gameSize.setSize(sf::Vector2f(0, 0));
		else {

			//Calculate game boundaries
			if(width > gameSize.getSize().x)
				gameSize.setSize(sf::Vector2f(width, gameSize.getSize().y));
			if(height > gameSize.getSize().y)
				gameSize.setSize(sf::Vector2f(gameSize.getSize().x, height));
		}

	}
};