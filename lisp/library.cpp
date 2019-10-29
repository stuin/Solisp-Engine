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
	library[NUMBER]["Set"] = [](marker pos, marker end) {
		string name = str_eval(*pos++);
		int value = num_eval(*pos++);
		env[name] = value;
		return value;
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
}