#include "Pointer.h"
//#include "../Network/GameClient.hpp"

class Root : public Node {
private:
	//Game objects
	StackRenderer *themeView = NULL;
	Pointer *pointer = NULL;

	//Saving var
	string saveFile;
	string ruleFile;

public:
	Root() : Node(BACKGROUND) {
		setPosition(100, 30);
		pointer = new Pointer(this);

		StackRenderer::cardScaling = 0.75;

		//Add decorational Cards
		Solisp::Stack *stack = new Solisp::Stack();
		stack->set_max(5);
		stack->add_count(5);
		stack->set_tag(SPREAD);
		stack->set_tag(SPREAD_HORIZONTAL);
		stack->set_cords(7, 7, 3, 8);
		stack->set_card(new Solisp::Card({1,'S', 4,'D', 7,'C', 12,'H', 14,'B'}, 8));
		themeView = new StackRenderer(stack, 1, NULL, DISPLAY, 1.3);
		themeView->reload();
	}

	void startGame(string _ruleFile, string _saveFile) {
		Solisp::Game *game = &Pointer::game;
		this->ruleFile = _ruleFile;

		if(_saveFile == "") {
			//Initialize game
			Solisp::Builder builder(_ruleFile);
			game->setup(&builder);
		} else {
			game->load(_saveFile, _ruleFile);
			std::remove(_saveFile.c_str());
		}

		//Get next save file name
		int startI = ruleFile.find_last_of(slash);
		saveFile = "saves/";
		saveFile += ruleFile.substr(startI, ruleFile.length() - startI - 6);
		int n = 1;
		while(std::filesystem::exists(saveFile + std::to_string(n) + ".sav"))
			++n;
		saveFile += std::to_string(n) + ".sav";

		//Set up slots
		Pointer::stacks.clear();
		Pointer::stacks.reserve((int)game->get_stack_count());
		Pointer::stacks.push_back(new StackRenderer(game->get_stack(0), 0, this, POINTER));
		for(unc i = 1; i < game->get_stack_count(); i++)
			Pointer::stacks.push_back(new StackRenderer(
				game->get_stack(i), i, this));
		UpdateList::sendSignal(STARTGAME);
		UpdateList::sendSignal(RELOADCARDS);

		//Final setup
		themeView->setHidden(true);
		pointer->reset(Pointer::stacks[0]);
	}

	void joinServer(string ip) {
		/*GameClient *client = new GameClient();
		if(client->connect(ip, &game)) {
			//Set up slots
			stacks.clear();
			stacks.reserve((int)game.get_stack_count());
			stacks.push_back(new StackRenderer(game.get_stack(0), 0, this, POINTER));
			for(unc i = 1; i < game.get_stack_count(); i++)
				stacks.push_back(new StackRenderer(game.get_stack(i), i, this));
			reloadAll();

			//Final setup
			themeView->setHidden(true);
			pointer->reset(stacks[0], client, client->get_user());
			showMenu(ACTIONMENU, false);
		}*/
	}

	void quitGame(bool save) {
		if(save)
			Pointer::game.save(saveFile);

		for(StackRenderer *s : Pointer::stacks)
			s->setDelete();
		Pointer::game.clear();
		UpdateList::sendSignal(ENDGAME);

		//Show menu
		pointer->setHidden(true);
		themeView->setHidden(false);
	}

	void restartGame() {
		quitGame(false);
		startGame(ruleFile, "");
	}
};