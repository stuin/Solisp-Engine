#include "../Gameplay/game.h"

struct node {
	//Node stats
	unsigned int depth;
	unsigned int simulations;
	unsigned int successes;
	unsigned int remaining;
	bool clean;

	//Node pointers
	node *parent;
	std::vector<node *> children;

	//Move
	unsigned int count;
	unc from;
	unc to;

	//Constructor
	node(unsigned int depth, unsigned int remaining, node *parent, unsigned int count, unc from, unc to) {
		this->depth = depth;
		this->parent = parent;

		this->count = count;
		this->from = from;
		this->to = to;

		//Blank initial values
		this->simulations = 0;
		this->successes = 0;
		this->clean = true;
	}

	//Push to end of child list
	void add_child(unsigned int remaining, unsigned int count, unc from, unc to) {
		this->children.push_back(new node(depth + 1, remaining, this, count, from, to));
	}
};

int INPUTS[8] = {6, 8, 9, 10, 11, 12, 13, 14};
int OUTPUTS[7] = {8, 9, 10, 11, 12, 13, 14};

int successes = 0;
int simulations = 0;

//Find all possible moves from the current node
void add_possibilities(Solisp::Game *game, node *root, int focus) {
	int remaining = game->get_remaining();
	root->clean = false;

	//Check for win
	if(remaining == 0) {
		root->successes++;
		successes++;
		return;
	}

	//Cancel invalid focus
	if(focus < 8) {
		for(int i = 8; i <= 14; i++)
			add_possibilities(game, root, i);
		return;
	}

	//Use draw button
	root->add_child(remaining, 1, 5, 5);

	//From others to focus
	for(int i : INPUTS) {
		if(i != focus && game->get_stack(i)->get_count() > 0) {
			//Try all shown cards in spread
			int count = 1;
			while(game->grab(count, i, 1)) {
				if(game->test(focus, 1))
					root->add_child(remaining, count, i, focus);
				count++;
			}
		}
	}

	//Check top card of focus
	if(game->grab(1, focus, 1)) {
		//Get max card count
		int max = 1;
		while(game->grab(max + 1, focus, 1))
			max++;

		//From focus to others
		for(int i : OUTPUTS) {
			if(i != focus && game->get_stack(i)->get_count() > 0) {
				for(int j = 0; j < max; j++)
					if(game->grab(j, focus, 1) && game->test(i, 1))
						root->add_child(remaining, j, i, focus);
			}
		}
	}
}

bool simulate(Solisp::Game *game, node *root) {
	//Run draw cards
	if(root->from == 5) {
		game->grab(1, 5, 1);
		simulations++;
		return true;
	}

	//Simulate actual move
	if(game->grab(root->count, root->from, 1) && game->place(root->to, 1)) {
		simulations++;
		return true;
	}

	std::cerr << "ERROR: Cannot simulate move from " << (int)root->from << " to " << (int)root->to << "\n";
	return false;
}
