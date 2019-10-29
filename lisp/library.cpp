#include "lisp.h"

/*
 * Created by Stuart Irwin on 29/10/2019.
 * Library functions for lisp system.
 */

void build_library() {
	//Add base string functions
	library[STRING]["+"] = [](marker pos, marker end) {
		string value;
		while (pos != end) 
			value += str_eval(*pos++);
		return cell(value);
	};
	library[STRING]["*"] = library[STRING]["+"];

	//Add base number functions
	library[NUMBER]["+"] = [](marker pos, marker end) {
		int value = 0;
		while(pos != end) 
			value += num_eval(*pos++);
		return cell(value);
	};
	library[NUMBER]["-"] = [](marker pos, marker end) {
		int value = num_eval(*pos++);
		while(pos != end) 
			value -= num_eval(*pos++);
		return cell(value);
	};
	library[NUMBER]["*"] = [](marker pos, marker end) {
		int value = 1;
		while(pos != end) 
			value *= num_eval(*pos++);
		return cell(value);
	};
	library[NUMBER]["=="] = [](marker pos, marker end) {
		int source = num_eval(*pos++);
		while(pos != end)
			if(source != num_eval(*pos++)) 
				return cell(0);
		return cell(1);
	};
	library[NUMBER]["Num"] = [](marker pos, marker end) {
		return num_eval(*pos);
	};

	//Add other general functions
	library[EXPR]["If"] = [](marker pos, marker end) {
		if(num_eval(*pos++)) {
			return *pos;
		} else {
			pos++;
			if(pos != end)
				return *pos;
			return cell(0);
		}
	};
	library[EXPR]["For-Each"] = [](marker pos, marker end) {
		pos++;
		int array[] = {2, 2, 2};
		string var = str_eval(*pos++);
		sexpr *output = new sexpr();
		output->push_back(cell("*"));

		//Combine each value
		for(int i : array) {
			env[var] = i;
			output->push_back(eval(*pos, NUMBER));
		}
		return cell(*output);
	};
	library[EXPR]["Set"] = [](marker pos, marker end) {
		string name = str_eval(*pos++);
		return env[name] = eval(*pos, EXPR);
	};
}