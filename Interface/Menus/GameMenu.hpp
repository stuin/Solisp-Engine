#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

#include "StartButton.hpp"

class GameMenu : public DrawNode {
private:
	std::vector<StartButton> games;
	sf::RectangleShape fade;

public:
	GameMenu(Node *parent) : DrawNode(fade, FADE, sf::Vector2i(700, 1090), parent) {
		setPosition(700, 0);
		setOrigin(0, 0);
		setHidden(true);
		UpdateList::addNode(this);

		//Set up faded background
		fade.setSize(sf::Vector2f(700, 1090));
		fade.setFillColor(sf::Color(0, 0, 0, 50));

		//Set up game list
		int y = 0;
		games.reserve(10);
		for(auto &entry : fs::directory_iterator("Games")) {
			if(entry.is_regular_file() && entry.path().extension().string() ==".solisp") {
				games.emplace_back(entry.path().relative_path().string(), y += 70, this);
			}
		}
	}
};