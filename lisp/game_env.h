/*
 * Created by Stuart Irwin on 28/1/2020.
 * Lisp structures for moving cards during game
 */

using Solisp::Move;

#define genv ((GameEnviroment*)env)

class GameEnviroment : public Enviroment {
private:
	//Generate base functions
	cell general_move(int num, bool flip, bool player);
	cell soft_move(int num);

	void build_library_game();

	int stack = 0;
	char STACKCOUNT;
	Solisp::Stack *stacks;
	Move *current;
	std::function<void(void)> update;

public:
	//Check if both slots in range
	bool both_valid(int from, int to) {
		return from > 0 && to > 0 &&
			from < STACKCOUNT && to < STACKCOUNT;
	}

	//Add new move to game
	bool add_move(int count, int from, int to, bool player, bool flip) {
		cout << "Moving " << count << " cards from " << from << " to " << to << "\n";

		if(player) {
			if(stacks[to].matches(count, stacks[from].get_card()))
				*current += new Move(from, to, count, 1, flip, current);
			else
				return false;
		} else
			*current += new Move(from, to, count, 0, flip, current);
		return true;
	}

	//Retrieve stack properties
	Solisp::Stack *get_stack(int i) {
		return &stacks[i];
	}

	//Set this value before evaluating
	bool run(cell c, int stack, int from, Move *current) {
		if(std::get<sexpr>(c.content).size() == 0)
			return false;

		//Update enviroment
		bool output = true;
		this->current = current;
		set("from", from);
		set("to", from);
		set("prev", this->stack);
		set("this", stack);
		this->stack = stack;

		//Attempt evaluation
		try {
			output = num_eval(c);
		} catch(std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			std::cerr << str_eval(c, true) << "\n";
		}

		return output;
	}

	//Build up lisp enviroment
	void setup(Solisp::Stack *stacks, int STACKCOUNT, std::function<void(void)> update) {
		this->STACKCOUNT = STACKCOUNT;
		this->stacks = stacks;
		this->update = update;

		//Build tag stack lists
		sexpr tags[STACKTAGCOUNT];
		for(int t = 0; t < STACKTAGCOUNT; t++) {
			for(int i = 0; i < STACKCOUNT; i++)
				if(stacks[i].get_tag(t))
					tags[t].push_back(cell(i));
		}

		//Link strings to stack tags
		auto it = Solisp::Stack::tag_map.begin();
		while(it != Solisp::Stack::tag_map.end()) {
			set(it->first, cell(tags[it->second], LIST));
			it++;
		}

		build_library_game();
		shift_env(true);
	}
};