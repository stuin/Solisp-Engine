#include "lisp.h"
#include "reader.h"

/*
 * Created by Stuart Irwin on 29/10/2019.
 * Library functions for lisp system.
 */

#define DONE if(pos != end) throw std::domain_error("Too many arguments: " + str_eval(*pos, true));

//Build a number comparison function
template <class T> builtin comparitor(T func) {
	return [func](marker pos, marker end) {
		int source = num_eval(*pos++);
		while(pos != end)
			if(!func(source, num_eval(*pos++)))
				return 0;
		return 1;
	};
}

//Build a number arithmatic function
template <class T> builtin arithmetic(T func) {
	return [func](marker pos, marker end) {
		int value = num_eval(*pos++);
		while(pos != end) 
			value = func(value, num_eval(*pos++));
		return value;
	};
}

//Build a function to force set the type of a structure
builtin forcer(cell_type type) {
	return [type](marker pos, marker end) {
		cell output = force_eval[type](*pos++);
		DONE;
		return output;
	};
}

void build_library() {
	//Build force evaluators
	force_eval[EXPR] = [](cell const &c) {
		return c;
	};
	force_eval[STRING] = [](cell const &c) {
		return cell(str_eval(c, false), STRING);
	};
	set_force_eval(&num_eval, NUMBER);
	set_force_eval(&list_eval, LIST);

	//Link force evaluators
	library[STRING]["Str"] = forcer(STRING);
	library[NUMBER]["Num"] = forcer(NUMBER);
	library[LIST]["List"] = forcer(LIST);

	//String concatanation
	library[STRING]["+"] = [](marker pos, marker end) {
		string value;
		while (pos != end) 
			value += str_eval(*pos++);
		return value;
	};

	//Basic arithmatic
	library[NUMBER]["+"] = arithmetic(std::plus<int>());
	library[NUMBER]["-"] = arithmetic(std::minus<int>());
	library[NUMBER]["*"] = arithmetic(std::multiplies<int>());
	library[NUMBER]["/"] = arithmetic(std::divides<int>());
	library[NUMBER]["%"] = arithmetic(std::modulus<int>());

	//Numerical comparisons
	library[NUMBER][">"] = comparitor(std::greater<int>());
	library[NUMBER]["<"] = comparitor(std::less<int>());

	//List building functions
	library[LIST]["Quote"] = [](marker pos, marker end) {
		sexpr *output = new sexpr();
		while(pos != end) 
			output->push_back(eval(*pos++, EXPR));
		return cell(*output, LIST);
	};
	library[LIST]["+"] = [](marker pos, marker end) {
		sexpr *output = new sexpr();
		while(pos != end) {
			sexpr array = list_eval(*pos++);
			output->insert(output->end(), array.begin(), array.end());
		}
		DONE;
		return cell(*output, LIST);
	};
	library[LIST]["-"] = [](marker pos, marker end) {
		sexpr *output = new sexpr();
		cell arraycell = *pos;
		sexpr array = list_eval(*pos++);
		cell remove = eval(*pos++, EXPR);
		
		//Copy all non-matching cells
		for(cell c : array) {
			if(!(remove == force_eval[remove.type](c)))
				output->push_back(c);
		}
		DONE;
		return cell(*output, LIST);
	};
	library[LIST]["*"] = [](marker pos, marker end) {
		sexpr *output = new sexpr();
		int count = num_eval(*pos++);
		sexpr array = list_eval(*pos++);

		for(int i = 0; i < count; i++)
			output->insert(output->end(), array.begin(), array.end());
		
		DONE;
		return cell(*output, LIST);
	};
	library[LIST]["Reverse"] = [](marker pos, marker end) {
		sexpr output = list_eval(*pos++);
		std::reverse(output.begin(),output.end());
		DONE;
		return cell(output, LIST);
	};

	//Control flow
	library[EXPR]["If"] = [](marker pos, marker end) {
		cell output = cell(0);
		if(num_eval(*pos++)) {
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
	library[EXPR]["For-Each"] = [](marker pos, marker end) {
		sexpr array = list_eval(*pos++);
		string var = str_eval(*pos++);
		sexpr *output = new sexpr();

		//Re list each value
		for(cell c : array) {
			env[var] = c;
			output->push_back(eval(*pos, EXPR));
		}
		pos++;
		DONE;
		return cell(*output, LIST);
	};
	library[EXPR]["Map"] = library[EXPR]["For-Each"];

	//Run data as code
	library[EXPR]["Eval"] = [](marker pos, marker end) {
		cell c = cell(list_eval(*pos++), EXPR);
		DONE;
		return c;
	};
	library[EXPR]["Read"] = [](marker pos, marker end) {
		cell c = read(str_eval(*pos++));
		DONE;
		return c;
	};

	//Variable management
	library[EXPR]["Set"] = [](marker pos, marker end) {
		string name = str_eval(*pos++);
		cell output = env[name] = *pos++;
		DONE;
		return output;
	};
	library[EXPR]["Def"] = library[EXPR]["Set"];
	library[EXPR]["Get"] = [](marker pos, marker end) {
		string name = str_eval(*pos++);
		DONE;
		return env[name];
	};

	//Universal comparisons
	library[EXPR]["=="] = [](marker pos, marker end) {
		cell c = eval(*pos++, EXPR);
		while(pos != end) {
			if(!(c.content == force_eval[c.type](*pos++).content))
				return 0;
		}
		return 1;
	};
	library[EXPR]["!="] = [](marker pos, marker end) {
		cell c = eval(*pos++, EXPR);
		while(pos != end)
			if(c.content == force_eval[c.type](*pos++).content)
				return 0;
		return 1;
	};

	if(addons)
		build_library_cont();
}