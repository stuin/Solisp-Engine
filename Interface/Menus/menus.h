#if __linux__
	#define slash "/"
#else
	#define slash "\\"
#endif

//Read game title from file
string GameNamer(string file) {
	std::ifstream input(file);
	std::string title;
	std::getline(input, title);
	return title.substr(0, title.find(','));
}

//Function to start new game
clickptr GameFunc(string file, Node *parent) {
	return [file, parent]() {
		startGame(file);
		parent->setHidden(true);
	};
}

//Format file name
string ThemeNamer(string file) {
	file = file.substr(file.find_last_of(slash) + 1, file.length() - 4);

	bool space = true;
	for(int i = 0; i < (int)file.length(); i++) {
		if(file[i] == '_') {
			space = true;
			file[i] = ' ';
		} else if(space) {
			space = false;
			file[i] = toupper(file[i]);
		}
	}
	return file;
}

//Function to start new game
clickptr ThemeFunc(string file, Node *parent) {
	return [file]() {
		changeCardset(file);
	};
}