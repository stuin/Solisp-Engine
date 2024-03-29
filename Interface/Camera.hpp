#include "main.h"

class Camera : public Node {
private:
	float gameWidth = 0;
	float gameHeight = 0;
	float screenWidth = 0;
	float screenHeight = 0;

	sf::RectangleShape rect;
	DrawNode background;

public:

	Camera() : Node(CAMERA, sf::Vector2i(1920, 1080)), rect(sf::Vector2f(1930, 1090)), background(rect, BACKGROUND) {
		setPosition(1920 / 2, 1080 / 2);

		//Draw background
		rect.setFillColor(sf::Color(7, 99, 36));
		UpdateList::addNode(&background);

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
			StackRenderer::cardScaling = (scaleX > scaleY) ? scaleY : scaleX;
			std::cout << "Scaling = " << StackRenderer::cardScaling << "\n";
			UpdateList::sendSignal(RELOADCARDS);
		}

		rect.setSize(sf::Vector2f(screenWidth + 10, screenHeight + 10));
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
			StackRenderer::cardScaling = (scaleX > scaleY) ? scaleY : scaleX;
		}
	}

	sf::Vector2f getCorner() {
		return sf::Vector2f(screenWidth, screenHeight);
	}
};