#include "lisp.h"

/*
 * Created by Stuart Irwin on 29/10/2019.
 * Library functions for lisp system.
 */

//Build a number comparison function
template <class T> builtin Enviroment::comparitor(T func) {
	return [func](Enviroment *env, marker pos, marker end) {
		int source = env->num_eval(*pos++);
		while(pos != end)
			if(!func(source, env->num_eval(*pos++)))
				return 0;
		return 1;
	};
}

//Build a number arithmatic function
template <class T> builtin Enviroment::arithmetic(T func) {
	return [func](Enviroment *env, marker pos, marker end) {
		int value = env->num_eval(*pos++);
		while(pos != end)
			value = func(value, env->num_eval(*pos++));
		return value;
	};
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
	library["+"] = arithmetic(std::plus<int>());
	library["-"] = arithmetic(std::minus<int>());
	library["*"] = arithmetic(std::multiplies<int>());
	library["/"] = arithmetic(std::divides<int>());
	library["%"] = arithmetic(std::modulus<int>());

	//Numerical comparisons
	library[">"] = comparitor(std::greater<int>());
	library["<"] = comparitor(std::less<int>());

	//List building functions
	library["Quote"] = [](Enviroment *env, marker pos, marker end) {
		sexpr output;
		while(pos != end)
			output.push_back(env->eval(*pos++, EXPR));
		return cell(output, LIST);
	};
	library["Append"] = [](Enviroment *env, marker pos, marker end) {
		sexpr output;
		while(pos != end) {
			sexpr array = env->list_eval(*pos++);
			output.insert(output.end(), array.begin(), array.end());
		}
		DONE;
		return cell(output, LIST);
	};
	library["Remove"] = [](Enviroment *env, marker pos, marker end) {
		sexpr array = env->list_eval(*pos++);
		cell remove = env->eval(*pos++, EXPR);
		sexpr output;

		//Copy all non-matching cells
		for(cell c : array) {
			if(!(remove == env->force_eval[remove.type](env, c)))
				output.push_back(c);
		}
		DONE;
		return cell(output, LIST);
	};
	library["Duplicate"] = [](Enviroment *env, marker pos, marker end) {
		int count = env->num_eval(*pos++);
		sexpr array = env->list_eval(*pos++);
		sexpr output;

		for(int i = 0; i < count; i++)
			output.insert(output.end(), array.begin(), array.end());

		DONE;
		return cell(output, LIST);
	};
	library["Reverse"] = [](Enviroment *env, marker pos, marker end) {
		sexpr output = env->list_eval(*pos++);
		std::reverse(output.begin(),output.end());
		DONE;
		return cell(output, LIST);
	};
	library["Join"] = [](Enviroment *env, marker pos, marker end) {
		sexpr array = env->list_eval(*pos++);
		string output;

		//Add deliminator if provided
		string delim = "";
		if(pos != end)
			delim += env->str_eval(*pos++);

		//Perform actual appending
		for(cell s : array)
			output += env->str_eval(s, false) + delim;

		DONE;
		return output;
	};

	//Control flow
	library["If"] = [](Enviroment *env, marker pos, marker end) {
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
	};
	library["For-Each"] = [](Enviroment *env, marker pos, marker end) {
		sexpr array = env->list_eval(*pos++);
		string var = env->str_eval(*pos++);
		sexpr output;

		//Re list each value
		for(cell c : array) {
			env->vars[var] = c;
			output.push_back(env->eval(*pos, EXPR));
		}
		pos++;
		DONE;
		return cell(output, LIST);
	};
	library["Map"] = library["For-Each"];

	//Run data as code
	library["Eval"] = [](Enviroment *env, marker pos, marker end) {
		cell c = cell(env->list_eval(*pos++), EXPR);
		DONE;
		return c;
	};

	//Variable management
	library["Set"] = [](Enviroment *env, marker pos, marker end) {
		string name = env->str_eval(*pos++);
		cell output = env->vars[name] = *pos++;
		DONE;
		return output;
	};
	library["Def"] = library["Set"];
	library["Get"] = [](Enviroment *env, marker pos, marker end) {
		string name = env->str_eval(*pos++);
		DONE;
		return env->vars[name];
	};

	//Universal comparisons
	library["=="] = [](Enviroment *env, marker pos, marker end) {
		cell c = env->eval(*pos++, EXPR);
		while(pos != end) {
			if(!(c.content == env->force_eval[c.type](env, *pos++).content))
				return 0;
		}
		return 1;
	};
	library["!="] = [](Enviroment *env, marker pos, marker end) {
		cell c = env->eval(*pos++, EXPR);
		while(pos != end)
			if(c.content == env->force_eval[c.type](env, *pos++).content)
				return 0;
		return 1;
	};
}