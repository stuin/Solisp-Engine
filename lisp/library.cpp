#include "lisp.h"

/*
 * Created by Stuart Irwin on 29/10/2019.
 * Library functions for lisp system.
 */

#define DONE if(pos != end) throw std::domain_error("Too many arguments")

template <class T> builtin comparitor(T func) {
	return [func](marker pos, marker end) {
		int source = num_eval(*pos++);
		while(pos != end)
			if(!func(source, num_eval(*pos++)))
				return 0;
		return 1;
	};
}
template <class T> builtin arithmetic(T func) {
	return [func](marker pos, marker end) {
		int value = num_eval(*pos++);
		while(pos != end) 
			value = func(value, num_eval(*pos++));
		return value;
	};
}

void build_library() {
	//Base string functions
	library[STRING]["+"] = [](marker pos, marker end) {
		string value;
		while (pos != end) 
			value += str_eval(*pos++);
		return value;
	};
	library[NUMBER]["Str"] = [](marker pos, marker end) {
		cell output = str_eval(*pos++);
		DONE;
		return output;
	};

	//Basic arithmatic
	library[NUMBER]["+"] = arithmetic(std::plus<int>());
	library[NUMBER]["-"] = arithmetic(std::minus<int>());
	library[NUMBER]["*"] = arithmetic(std::multiplies<int>());
	library[NUMBER]["/"] = arithmetic(std::divides<int>());
	library[NUMBER]["%"] = arithmetic(std::modulus<int>());

	//Numerical comparisons
	library[NUMBER]["=="] = comparitor(std::equal_to<int>());
	library[NUMBER]["!="] = comparitor(std::not_equal_to<int>());
	library[NUMBER][">"] = comparitor(std::greater<int>());
	library[NUMBER]["Num"] = [](marker pos, marker end) {
		cell output = num_eval(*pos++);
		DONE;
		return output;
	};

	//List building functions
	library[LIST]["Steps"] = [](marker pos, marker end) {
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
		return cell(*output);
	};
	library[NUMBER]["List"] = [](marker pos, marker end) {
		cell output = list_eval(*pos++);
		DONE;
		return output;
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
			output->push_back(eval(*pos++, NUMBER));
		}
		DONE;
		return cell(*output, LIST);
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
}