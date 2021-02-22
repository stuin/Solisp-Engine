//Read game title from file
string GameNamer(string file) {
	std::ifstream input(file);
	std::string title;
	std::getline(input, title);
	return title.substr(0, title.find(','));
}

//Format file name
string ThemeNamer(string file) {
	bool space = true;
	int startI = 0;

	for(int i = 0; i < (int)file.length(); i++) {
		if(file[i] == slash) {
			startI = i + 1;
			space = true;
		} else if(file[i] == '_' || file[i] == '.') {
			space = true;
			file[i] = ' ';
		} else if(space) {
			space = false;
			file[i] = toupper(file[i]);
		}
	}
	return file.substr(startI, file.length() - startI - 4);
}

//Function to start new game
clickptr GameFunc(string file, Node *parent) {
	return [file, parent]() {
		startGame(file, "");
		showMenu(ACTIONMENU, false);
	};
}

//Function to change card texture
clickptr ThemeFunc(string file, Node *parent) {
	return [file]() {
		if(!getCardset()->loadFromFile(file))
			throw std::invalid_argument("Card texture not found");
	};
}

//Function to open save file and start game
clickptr LoadFunc(string file, Node *parent) {
	return [file, parent]() {
		string rule_file = "Games";
		int startI = file.find_last_of(slash);
		rule_file += file.substr(startI, file.find_first_of(".") - startI);
		rule_file += ".solisp";

		startGame(rule_file, file);
		showMenu(ACTIONMENU, false);
	};
}