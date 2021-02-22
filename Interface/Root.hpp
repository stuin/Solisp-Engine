#include "Pointer.hpp"

class Root : public Node {
private:
	//Game objects
	StackRenderer *themeView = NULL;
	Pointer *pointer = NULL;

	//Saving var
	string next_save_file;

public:
	Root() : Node(BACKGROUND) {
		setPosition(100, 30);

		//Add decorational Cards
		Solisp::Stack *stack = new Solisp::Stack();
		stack->set_max(5);
		stack->add_count(5);
		stack->set_tag(SPREAD);
		stack->set_tag(SPREAD_HORIZONTAL);
		stack->set_card(new Solisp::Card({1,'S', 4,'D', 7,'C', 12,'H', 1,'J'}, 8));
		themeView = new StackRenderer(stack, 1, this, DISPLAY, 1.3);
		themeView->setPosition(700, 400);
	}

	void startGame(string rule_file, string save_file) {
		if(save_file == "") {
			//Initialize game
			Solisp::Builder builder(rule_file);
			game.setup(&builder);
			game.update();
		} else {
			game.load(save_file, rule_file);
			std::remove(save_file.c_str());
		}

		//Get next save file name
		int startI = rule_file.find_last_of(slash);
		next_save_file = "saves/";
		next_save_file += rule_file.substr(startI, rule_file.length() - startI - 6);
		int n = 1;
		while(std::filesystem::exists(next_save_file + std::to_string(n) + ".sav"))
			++n;
		next_save_file += std::to_string(n) + ".sav";

		//Set up slots
		stacks.clear();
		stacks.reserve((int)game.get_stack_count());
		stacks.push_back(new StackRenderer(game.get_stack(0), 0, this, POINTER));
		for(unc i = 1; i < game.get_stack_count(); i++)
			stacks.push_back(new StackRenderer(game.get_stack(i), i, this));
		reloadAll();

		//Final setup
		themeView->setHidden(true);

		if(pointer == NULL) {
			pointer = new Pointer(stacks[0], this);
			UpdateList::addNode(pointer);
		} else
			pointer->reset(stacks[0]);
	}

	void quitGame(bool save) {
		if(save)
			game.save(next_save_file);

		for(StackRenderer *s : stacks)
			s->setDelete();
		game.clear();

		//Show menu
		pointer->setHidden(true);
		themeView->setHidden(false);
		showMenu(0, false);
	}
};