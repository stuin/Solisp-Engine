/*
 * Created by Stuart Irwin on 28/1/2020.
 * Lisp structures for moving cards during game
 */

using Solisp::Move;

#define genv ((GameEnviroment*)env)

class GameEnviroment : public Enviroment {
private:
	//Generate base functions
	cell general_move(int num, bool flip, unc player);
	cell soft_move(int num);

	void build_library_game();

	int stack = 0;
	char STACKCOUNT;

	//Game functions
	Solisp::Stack *stacks;
	std::function<void(Move)> apply;

public:
	//Check if both slots in range
	bool both_valid(unc from, unc to) {
		return from > 0 && to > 0 &&
			from < STACKCOUNT && to < STACKCOUNT;
	}

	bool check_move(unsigned int count, unc from, unc to) {
		return both_valid(from, to) && stacks[from].get_count() >= count &&
			(from == to || stacks[to].matches(count, stacks[from].get_card()));
	}

	//Add new move to game
	void add_move(unsigned int count, unc from, unc to, unc user, bool flip) {
		//if(user == 1) cout << "Try: ";
		//cout << "Moving " << count << " cards from " << from << " to " << to << "\n";

		apply({from, to, count, user, flip});
	}

	//Retrieve stack properties
	Solisp::Stack *get_stack(int i) {
		return &stacks[i];
	}

	//Set this value before evaluating
	bool run(cell c, int stack, int other) {
		if(std::get<sexpr>(c.content).size() == 0)
			return false;

		//Update enviroment
		shift_env(true);
		set("from", other);
		set("to", other);
		set("prev", this->stack);
		set("this", stack);
		this->stack = stack;

		//Attempt evaluation
		bool output = true;
		try {
			output = num_eval(c);
		} catch(std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			std::cerr << str_eval(c, true) << "\n";
		}
		shift_env(false);

		return output;
	}

	//Build up lisp enviroment
	void setup(Solisp::Stack *stacks, int STACKCOUNT, std::function<void(Move)> apply) {
		this->STACKCOUNT = STACKCOUNT;
		this->stacks = stacks;
		this->apply = apply;

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
	}
};