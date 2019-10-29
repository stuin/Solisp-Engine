#include "lisp.h"

void build_library() {
	//Add base string functions
	library[STRING]["+"] = [](marker pos, marker end) {
		string ret;
		while (pos != end) ret += str_eval(*pos++);
		return cell(ret);
	};

	//Add base number functions
	library[NUMBER]["+"] = [](marker pos, marker end) {
		int ret = 0;
		while(pos != end) ret += num_eval(*pos++);
		return cell(ret);
	};
	library[NUMBER]["-"] = [](marker pos, marker end) {
		int ret = num_eval(*pos++);
		while(pos != end) ret -= num_eval(*pos++);
		return cell(ret);
	};
	library[NUMBER]["*"] = [](marker pos, marker end) {
		int ret = 1;
		while(pos != end) ret *= num_eval(*pos++);
		return cell(ret);
	};

	//Add other general functions
	library[EXPR]["if"] = [](marker pos, marker end) {
		if(num_eval(*pos++))
			return cell(eval(*pos, EXPR));
		else {
			pos++;
			return cell(eval(*pos, EXPR));
		}
	};
}