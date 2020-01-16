#include "lisp.h"

/*
 * Created by Stuart Irwin on 29/10/2019.
 * library functions for lisp system.
 */

//Build a number comparison function
template <class T> cell Enviroment::comparitor(T func) {
	return cell([func](Enviroment *env, marker pos, marker end) {
		int source = env->num_eval(*pos++);
		while(pos != end)
			if(!func(source, env->num_eval(*pos++)))
				return 0;
		return 1;
	});
}

//Build a number arithmatic function
template <class T> cell Enviroment::arithmetic(T func) {
	return cell([func](Enviroment *env, marker pos, marker end) {
		int value = env->num_eval(*pos++);
		while(pos != end)
			value = func(value, env->num_eval(*pos++));
		return value;
	});
}

void Enviroment::build_library() {
	//Build force evaluators
	force_eval[EXPR] = [](Enviroment *env, cell const &c) {
		return c;
	};
	force_eval[STRING] = [](Enviroment *env, cell const &c) {
		return cell(env->str_eval(c, false), STRING);
	};
	force_eval[NUMBER] = [](Enviroment *env, cell const &c) {
		return cell(env->num_eval(c), NUMBER);
	};
	force_eval[CHAR] = [](Enviroment *env, cell const &c) {
		return cell(env->char_eval(c), CHAR);
	};
	force_eval[LIST] = [](Enviroment *env, cell const &c) {
		return cell(env->list_eval(c), LIST);
	};

	//Basic arithmatic
	set("+", arithmetic(std::plus<int>()));
	set("-", arithmetic(std::minus<int>()));
	set("*", arithmetic(std::multiplies<int>()));
	set("/", arithmetic(std::divides<int>()));
	set("%", arithmetic(std::modulus<int>()));

	//Numerical comparisons
	set(">", comparitor(std::greater<int>()));
	set("<", comparitor(std::less<int>()));

	//Advanced list to string conversion
	set("Join", cell([](Enviroment *env, marker pos, marker end) {
		sexpr array = env->list_eval(*pos++);
		string output;

		//Set deliminator if provided
		string delim = "";
		if(pos != end)
			delim += env->str_eval(*pos++);

		//Perform actual appending
		for(cell s : array)
			output += env->str_eval(s, false) + delim;

		DONE;
		return output;
	}));

	//List building functions
	set("Quote", cell([](Enviroment *env, marker pos, marker end) {
		sexpr output;
		while(pos != end)
			output.push_back(env->eval(*pos++));
		return cell(output, LIST);
	}));
	set("Append", cell([](Enviroment *env, marker pos, marker end) {
		sexpr output;
		while(pos != end) {
			sexpr array = env->list_eval(*pos++);
			output.insert(output.end(), array.begin(), array.end());
		}
		DONE;
		return cell(output, LIST);
	}));
	set("Remove", cell([](Enviroment *env, marker pos, marker end) {
		cell remove = env->eval(*pos++);
		sexpr array = env->list_eval(*pos++);
		sexpr output;

		//Copy all non-matching cells
		for(cell c : array) {
			if(!(env->equals(remove, c)))
				output.push_back(c);
		}
		DONE;
		return cell(output, LIST);
	}));
	set("Duplicate", cell([](Enviroment *env, marker pos, marker end) {
		int count = env->num_eval(*pos++);
		sexpr array = env->list_eval(*pos++);
		sexpr output;

		for(int i = 0; i < count; i++)
			output.insert(output.end(), array.begin(), array.end());

		DONE;
		return cell(output, LIST);
	}));
	set("Reverse", cell([](Enviroment *env, marker pos, marker end) {
		sexpr output = env->list_eval(*pos++);
		std::reverse(output.begin(),output.end());
		DONE;
		return cell(output, LIST);
	}));

	//Control flow
	set("If", cell([](Enviroment *env, marker pos, marker end) {
		cell output = cell(0);
		if(env->num_eval(*pos++)) {
			//If true
			output = *pos++;
			if(pos != end)
				pos++;
		} else {
			//If false
			pos++;
			if(pos != end)
				output = *pos++;
		}
		DONE;
		return output;
	}));
	set("Map", cell([](Enviroment *env, marker pos, marker end) {
		sexpr array = env->list_eval(*pos++);
		string var = env->str_eval(*pos++);
		sexpr output;
		env->shift_env(true);

		//Re list each value
		for(cell c : array) {
			env->set(var, c);
			output.push_back(env->eval(*pos));
		}

		env->shift_env(false);
		pos++;
		DONE;
		return cell(output, LIST);
	}));
	set("Step", cell([](Enviroment *env, marker pos, marker end) {
		sexpr array = env->list_eval(*pos++);
		cell output;

		for(cell c : array)
			output = env->eval(c);

		DONE;
		return output;
	}));

	//Convert list to runnable code
	set("Eval", cell([](Enviroment *env, marker pos, marker end) {
		cell c = cell(env->list_eval(*pos++), EXPR);
		DONE;
		return c;
	}));

	//Variable management
	set("Set", cell([](Enviroment *env, marker pos, marker end) {
		string name = env->str_eval(*pos++);
		cell output = env->set(name, *pos++);
		DONE;
		return output;
	}));
	set("Env", cell([](Enviroment *env, marker pos, marker end) {
		env->shift_env(true);
		cell output = env->eval(*pos++);

		env->shift_env(false);
		DONE;
		return output;
	}));

	//Universal comparisons
	set("==", cell([](Enviroment *env, marker pos, marker end) {
		cell c = env->eval(*pos++);
		while(pos != end) {
			if(!(env->equals(c, *pos++)))
				return 0;
		}
		return 1;
	}));
	set("!=", cell([](Enviroment *env, marker pos, marker end) {
		cell c = env->eval(*pos++);
		while(pos != end)
			if(env->equals(c, *pos++))
				return 0;
		return 1;
	}));
}