#include "../Gameplay/game.h"

struct node {
	//Node stats
	unsigned int depth;
	unsigned int simulations = 0;
	unsigned int remaining;
	bool clean = true;
	bool failed = false;

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
		this->remaining = remaining;

		this->parent = parent;

		this->count = count;
		this->from = from;
		this->to = to;
	}

	//Push to end of child list
	void add_child(unsigned int count, unc from, unc to) {
		this->children.push_back(new node(depth + 1, remaining, this, count, from, to));
	}

	void print_stack() {
		if(parent != NULL)
			parent->print_stack();
		//if(from != 5)
		cout << '{' << (int)from << ':' << (int)to << "} ";
	}
};

int INPUTS[8] = {6, 8, 9, 10, 11, 12, 13, 14};
int OUTPUTS[11] = {1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14};

int simulations = 0;

//Find all possible moves from the current node
void add_possibilities(Solisp::Game *game, node *root, int focus) {
	root->clean = false;

	//Cancel invalid focus
	if(focus < 8) {
		for(int i = 8; i <= 14; i++)
			add_possibilities(game, root, i);
		return;
	}

	//Use draw button
	root->add_child(1, 5, 5);

	//From others to focus
	for(int i : INPUTS) {
		if(i != focus && i != root->to && game->get_stack(i)->get_count() > 0) {
			//Try all shown cards in spread
			int count = 1;
			while(game->grab(count, i, 1)) {
				if(game->test(focus, 1))
					root->add_child(count, i, focus);
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
			if(i != focus && i != root->to && game->get_stack(i)->get_count() > 0) {
				for(int j = 0; j < max; j++)
					if(game->grab(j, focus, 1) && game->test(i, 1))
						root->add_child(j, focus, i);
			}
		}
	}
}

bool simulate(Solisp::Game *game, node *root) {
	if(root->failed)
		return false;

	//Run draw cards
	if(root->from == 5) {
		game->grab(0, 5, 1);
		simulations++;
		return true;
	}

	//Simulate actual move
	if(game->grab(root->count, root->from, 1) && game->place(root->to, 1)) {
		root->remaining = game->get_remaining();
		simulations++;
		return true;
	}

	root->failed = true;
	std::cerr << "ERROR: Cannot simulate move from " << (int)root->from << " to " << (int)root->to << " at depth " << root->depth << "\n";
	return false;
}
