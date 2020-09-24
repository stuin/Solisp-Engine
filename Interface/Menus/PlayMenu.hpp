#include <filesystem>

namespace fs = std::filesystem;

#include "StartButton.hpp"

class PlayMenu : public DrawNode {
private:
	std::vector<StartButton> games;
	sf::RectangleShape fade;

public:
	PlayMenu() : DrawNode(fade, FADE) {

		//Set up faded background
		fade.setSize(sf::Vector2f(700, 1090));
		fade.setFillColor(sf::Color(0, 0, 0, 50));

		//Set up game list
		int y = 0;
		games.reserve(8);
		for(auto &entry : fs::directory_iterator("Games")) {
			if(entry.is_regular_file() && entry.path().extension().string() ==".solisp") {
				games.emplace_back(entry.path().relative_path().string(), y += 100, this);
			}
		}
	}
};