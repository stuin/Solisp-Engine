#include "../Gameplay/game.h"

struct node {
	//Node stats
	unsigned int id;
	unsigned int simulations;
	unsigned int successes;
	unsigned int remaining;
	unsigned int children;

	//Node pointers
	node *parent;
	node *sibling;
	node *child;

	//Move
	unsigned int count;
	unc from;
	unc to;

	//Constructor
	node(unsigned int id, unsigned int remaining, node *parent, unsigned int count, unc from, unc to) {
		this->id = id;
		this->parent = parent;

		this->count = count;
		this->from = from;
		this->to = to;

		//Blank initial values
		this->simulations = 0;
		this->successes = 0;
		this->children = 0;
		this->sibling = NULL;
		this->child = NULL;
	}

	//Push to end of child list
	void add_child(node *child) {
		this->children++;
		if(this->child == NULL)
			this->child = child;
		else {
			//Get last sibling
			node *next = this->child;
			while(next->sibling != NULL)
				next = next->sibling;
			next->sibling = child;
		}
	}
}

//Find all possible moves from the current node
void add_possibilities(Solisp::Game *game, node *root, unc focus) {
	int id = root->id + 1;
	int remaining = game->cards_remaining();
	int size = game->get_stack_count();

	//Check for win
	if(remaining == 0) {
		root->successes++;
		root->simulations++;
		return;
	}

	//Cancel invalid focus
	if(focus == 0 || focus > size) {
		for(int i = 1; i < size; i++)
			add_possibilities(game, root, i);
		return;
	}
	root->simulations++;

	//Click focused button
	if(game->get_stack(focus)->get_tag(BUTTON)) {
		root->add_child(new node(id++, remaining, root, 1, focus, focus));
		return;
	}

	//Cancel hidden cards
	if(game->get_stack(focus)->is_hidden())
		return;

	//From others to focus
	for(int i = 1; i < size; i++) {
		if(i != focus && !game->get_stack(i)->get_tag(BUTTON) && !game->get_stack(i)->is_hidden()) {
			//Try all shown cards in spread
			int count = 1;
			while(game->grab(count, i, 1)) {
				if(game->test(focus, 1))
					root->add_child(new node(id++, remaining, root, count, i, focus));
				count++;
			}
		}
	}

	//Check top card
	if(game->grab(1, focus, 1)) {
		//Get max card count
		int max = 1;
		while(game->grab(max + 1, focus, 1))
			max++;

		//From focus to others
		for(int i = 1; i < size; i++) {
			if(i != focus && !game->get_stack(i)->get_tag(BUTTON) && !game->get_stack(i)->get_card()->is_hidden()) {
				for(int j = 0; j < max; j++)
					if(game->grab(j, focus, 1) && game->test(i, 1))
						root->add_child(new node(id++, remaining, root, j, i, focus));
			}
		}
	}
}

