#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

#include "Button.hpp"

class FolderMenu : public DrawNode {
private:
	std::vector<Button> games;
	sf::RectangleShape fade;

public:
	FolderMenu(string dir, string ext, function<string(string)> name,
			function<clickptr(string, Node*)> func, Node *parent)
			: DrawNode(fade, FADE, sf::Vector2i(700, 1090), parent) {
		setPosition(300, 0);
		setOrigin(0, 0);
		setHidden(true);
		UpdateList::addNode(this);

		//Set up faded background
		fade.setSize(sf::Vector2f(700, 1090));
		fade.setFillColor(sf::Color(0, 0, 0, 50));

		//Set up game list
		int y = 0;
		games.reserve(10);
		for(auto &entry : fs::directory_iterator(dir)) {
			if(entry.is_regular_file() && entry.path().extension().string() == ext) {
				string file = entry.path().relative_path().string();
				games.emplace_back(name(file), y += 70, 600, this, func(file, parent));
			}
		}
	}
};