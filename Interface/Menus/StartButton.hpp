#include <fstream>

#include "../Skyrmion/UpdateList.h"
#include "Button.hpp"

class StartButton : public Button {
private:
	string file;

public:
	StartButton(string file, int y, Node *parent) : Button(file, y, 600, parent) {
		this->file = file;
		std::cout << file << "\n";

		//Read game title
		std::ifstream input(file);
		std::string title;
		std::getline(input, title);
		setText(title);
	}

	void click() override {
		startGame(file);
		getParent()->getParent()->setHidden(true);
	}
};