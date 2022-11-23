#include "main.h"

class Camera : public Node {
private:
	float gameWidth = 0;
	float gameHeight = 0;
	float screenWidth = 0;
	float screenHeight = 0;

public:

	Camera() : Node(CAMERA, sf::Vector2i(1920, 1080)) {
		setPosition(1920 / 2, 1080 / 2);

		UpdateList::addNode(this);
		UpdateList::addListener(this, sf::Event::Resized);
	}

	void recieveEvent(sf::Event event, WindowSize *windowSize) {
		std::cout << event.size.width << ", " << event.size.height << "\n";
		screenWidth = event.size.width;
		screenHeight = event.size.height;

		//Resize game field
		if(gameWidth > 0) {
			float scaleX = screenWidth / gameWidth;
			float scaleY = screenHeight / gameHeight;
			cardScaling = (scaleX > scaleY) ? scaleY : scaleX;
			std::cout << "Scaling = " << cardScaling << "\n";
			UpdateList::sendSignal(RELOADGAME);
		}

		setPosition(screenWidth / 2, screenHeight / 2);
		UpdateList::setCamera(this, sf::Vector2f(screenWidth, screenHeight));
	}

	void setGameSize(int width, int height) {
		if(width == -1 && height == -1) {
			gameWidth = 0;
			gameHeight = 0;
		} else {
			//Calculate game boundaries
			if(width > gameWidth)
				gameWidth = width;
			if(height > gameHeight)
				gameHeight = height;

			float scaleX = screenWidth / gameWidth;
			float scaleY = screenHeight / gameHeight;
			cardScaling = (scaleX > scaleY) ? scaleY : scaleX;
		}
	}

	sf::Vector2f getCorner() {
		return sf::Vector2f(screenWidth, screenHeight);
	}
};