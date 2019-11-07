#include <list>
#include <iostream>
#include <fstream>

/*
 * Created by Stuart Irwin on 7/11/2019.
 * Functions for reading files into lisp structure.
 */

//Convert given string to list of tokens
std::list<std::string> tokenize(const std::string & str) {
    std::list<std::string> tokens;
    const char *s = str.c_str();
    while(*s) {
        while(*s == ' ' || *s == '\t')
            ++s;
        if(*s == '(' || *s == ')')
            tokens.push_back(*s++ == '(' ? "(" : ")");
        else if(*s == '{' || *s == '}')
            tokens.push_back(*s++ == '{' ? "{" : "}");
        else if(*s == '"') {
        	//Get full string
        	const char *t = ++s;
            while(*t && *t != '"')
                ++t;
            tokens.push_back(std::string(s, t));
            s = ++t;
        } else {
            const char * t = s;
            while(*t && *t != ' ' && *t != '\t' && *t != '(' && *t != ')' && *t != '{' && *t != '}')
                ++t;
            tokens.push_back(std::string(s, t));
            s = t;
        }
    }
    return tokens;
}

//Numbers become Numbers; every other token is a String
cell atom(const std::string & token) {
    if(isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        return cell(stoi(token));
    return cell(token);
}

//Return the Lisp expression in the given tokens
cell read_from(std::list<std::string> & tokens) {
    const std::string token(tokens.front());
    cell_type type = (token == "{") ? LIST : EXPR;
    tokens.pop_front();
    if(token == "(" || token == "{") {
        sexpr *output = new sexpr();
        while(tokens.front() != ")" && tokens.front() != "}")
            output->push_back(read_from(tokens));
        tokens.pop_front();
        return cell(*output, type);
    }
    else
        return atom(token);
}

//Return the Lisp expression represented by the given string
cell read(const std::string & s) {
    std::list<std::string> tokens(tokenize(s));
    return read_from(tokens);
}

//Read objects spanning multiple lines into one string
cell read_stream(std::istream &in, cell_type type) {
	std::string object;
	int levels = -1;
	bool literal = false;

	//For each incomplete line
	while((levels != 0 || object.length() < 1) && !in.eof()) {
		bool comment = false;
		std::string line;
		std::getline(in, line);
		if(line.length() > 0) {
			if(levels == -1)
				levels = 0;

			//For each letter
			for(char c : line) {
				if(!literal && !comment) {
					//Count parenthesis and lists
					if(c == '(' || c == '{') {
						levels++;
					} else if(c == ')' || c == '}') {
						levels--;
					} else if(c == '#') {
						if(object.length() > 0)
							object += ' ';
						comment = true;
					} 
				}
				if(c == '"')
					literal = !literal;
				if(!comment)
					object += c;
			}
			if(!comment)
				object += ' ';
		}
	}

	//Run actual evaluation with error checking
	try {
		if(object.length() > 0) {
			if(in.eof() && levels != 0)
				throw std::domain_error(std::to_string(levels) + " non matched parenthesis");
			
			return force_eval[type](read(object));
		}
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return cell("");
}