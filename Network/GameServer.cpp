#include "../Gameplay/filelist.h"
#include "TCP.hpp"

#include <iostream>

using std::cout;

class tcp_server {
public:
	tcp_server(boost::asio::io_context& io_context, Solisp::Game *game, string setup)
		: io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 23))	{

		this->game = game;
		this->setup = setup;
    	start_accept();
  	}

private:
	void start_accept() {
	    tcp_connection::pointer new_connection = tcp_connection::create(io_context_);
    	acceptor_.async_accept(new_connection->socket(),
    		boost::bind(&tcp_server::handle_accept, this, new_connection,
    		boost::asio::placeholders::error));
	}

	void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error) {
		if(!error) {
			new_connection->start(game, ++users, setup);
		}
		start_accept();
	}

	boost::asio::io_context& io_context_;
	tcp::acceptor acceptor_;
	Solisp::Game *game;
	string setup;
	int users = 0;
};

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

