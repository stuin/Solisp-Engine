//Engine headers
#include "../Skyrmion/UpdateList.h"
#include "../../Gameplay/game.h"

#include "Button.hpp"

class StartButton : public Button {
private:
	string file;

public:
	StartButton(string file, int y, Node *parent) : Button(file, y, parent) {
		this->file = file;
		std::cout << file << "\n";

		//Read game title
		std::ifstream input(file);
		std::string title;
		std::getline(input, title);
		setText(title);
	}

	void click() override {
		//Initialize game
		Solisp::Builder builder(file);
		game.setup(&builder);
		game.update();

		//Set up slots
		STACKCOUNT = game.get_stack_count();
		stacks.reserve(STACKCOUNT);
		stacks.emplace_back(game.get_stack(0), 0);
		for(int i = 1; i < STACKCOUNT; i++)
			stacks.emplace_back(game.get_stack(i), i);

		//Final setup
		UpdateList::addNode(new Pointer(&(stacks[0]), &game));
		getParent()->getParent()->setHidden(true);
	}
};