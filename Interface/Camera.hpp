#include "main.h"

class Camera : public Node {
	float gameWidth = 0;
	float gameHeight = 0;
	float screenWidth = 0;
	float screenHeight = 0;

public:
	Camera() : Node(POINTER, sf::Vector2i(1920, 1080)) {
		setPosition(1920 / 2, 1080 / 2);

		UpdateList::addNode(this);
		UpdateList::addListener(this, sf::Event::Resized);
	}

	void recieveEvent(sf::Event event, int shiftX, int shiftY) {
		std::cout << event.size.width << ", " << event.size.height << "\n";
		screenWidth = event.size.width;
		screenHeight = event.size.height;

		//Resize game field
		if(gameWidth > 0) {
			float scaleX = screenWidth / gameWidth;
			float scaleY = screenHeight / gameHeight;
			cardScaling = (scaleX > scaleY) ? scaleY : scaleX;
			std::cout << "Scaling = " << cardScaling << "\n";
			reloadAll();
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
			cardScaling = bet(0.25, cardScaling, 1.5);
		}
	}
};