namespace Solisp {
	class GameInterface;
	class Game;
	struct Hand;
}

#include "../Gameplay/builder.h"
#include "../Gameplay/move.h"

#include <stdexcept>

/*
 * Created by Stuart Irwin on 29/6/2019.
 * Game rules and management.
 */

struct Solisp::Hand {
	unc from = 0;
	unc to = 0;
	unc tested = 0;
	unsigned int count = 0;
};

enum Stage {
	NONE,
	STARTING,
	LOADING,
	PLAYING,
	END
};

class Solisp::GameInterface {
public:
	virtual bool grab(unsigned int num, unc from, unc user) { return false; }
	virtual bool test(unc to, unc user) { return false; }
	virtual bool place(unc to, unc user) { return false; }
	virtual void cancel(unc user) {}
};

class Solisp::Game : public GameInterface {
private:
	//Game definition
	unc STACKCOUNT = 0;
	Hand *users = NULL;
	Stage stage = NONE;

	//Current game state
	Stack stack[128];
	int cardsLeft = 0;

	//Apply current moves to stack array
	void apply(Move move, bool reverse);

	//Startup functions
	void shuffle();
	void deal();

public:
	unc players = 3;
	vector<Move> moves;

	Game() {}
	Game(const Game &other);
	~Game() {
		clear();
	}

	//Setup functions
	void setup(Builder *builder, bool saved=false);
	void clear();

	//General interaction methods
	bool grab(unsigned int num, unc from, unc user);
	bool test(unc to, unc user);
	bool place(unc to, unc user);
	void cancel(unc user);

	//Specialized interaction methods
	void apply(Move move, unsigned int server=0);
	void apply(unc from, unc to, unsigned int count, unc user, bool flip);
	void undo(unc user, bool anyone);
	void print_game();

	//File loading/saving
	void save(string file);
	void load(string file, string rule_file);

	//Stack access
	int get_stack_count() {
		return STACKCOUNT;
	}
	Stack *get_stack(unc i) {
		if(i >= STACKCOUNT)
			return NULL;
		return &stack[i];
	}

	//Current game state
	unsigned int get_remaining() {
		return cardsLeft;
	}
	Stage get_stage() {
		return stage;
	}

	void end_game(bool win) {
		if(win)
			cardsLeft = 0;
		stage = END;
	}
};
