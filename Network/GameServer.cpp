#include "../Gameplay/filelist.h"
#include "TCP.hpp"

#include <iostream>

using std::cout;

int main(int argc, char const *argv[]) {
	int game_number;
	if(argc > 1) {
		cout << "Game number = " << argv[1] << "\n";
		game_number = argv[1][0] - '0';
	} else {
		cout << "Game number: ";
		std::cin >> game_number;
	}

	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder(rule_files[game_number]);

	game.setup(builder);
	game.update();

	try {
		boost::asio::io_context io_context;
		tcp_server server(io_context, &game, rule_files[game_number]);
		io_context.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

