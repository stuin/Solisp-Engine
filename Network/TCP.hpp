#include "../Gameplay/game.h"

#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_context& io_context) {
	    return pointer(new tcp_connection(io_context));
	}

	tcp::socket& socket() {
		return socket_;
	}

	void start(Solisp::Game *game, int user, std::string setup) {
		this->game = game;
		this->user = user;

		//Send initial setup message
		boost::asio::async_write(socket_, boost::asio::buffer(setup),
			boost::bind(&tcp_connection::handle_read, shared_from_this(),
			boost::asio::placeholders::error));
	}

	void update() {
		if(syncing || current == NULL || game == NULL)
			return;

		if(current != game->get_current()) {
			syncing = true;
			send_update();
		}
	}

private:
	tcp_connection(boost::asio::io_context& io_context) : socket_(io_context) {
	}

	void handle_read(const boost::system::error_code& error) {
		if(!error) {
			//Read player move
			boost::asio::async_read(socket_, boost::asio::buffer(inbound),
	        	boost::bind(&tcp_connection::handle_read, shared_from_this(),
	            boost::asio::placeholders::error));
		}
	}

	void handle_update(const boost::system::error_code& error) {
		if(!error) {
			if(inbound[0] == -1)
				game->place(inbound[1], user);
			else
				game->grab(inbound[0], inbound[1], user);

			send_update();
		}
	}

	void send_update() {
		if(current == game->get_current()) {
			syncing = false;
			return;
		}

		//Shift backwards if needed
		while(!current->get_tag(VALID))
			current = current->get_last();

		if(current->get_next() != NULL) {
			current = current->get_next();

			//Serialize
			std::ostringstream archive_stream;
			boost::archive::text_oarchive archive(archive_stream);
			//archive << *current;

			//Send
			boost::asio::async_write(socket_, boost::asio::buffer(archive_stream.str()),
				boost::bind(&tcp_connection::handle_update, shared_from_this(),
				boost::asio::placeholders::error));
		}
	}

	tcp::socket socket_;

	//Inputs
	int inbound[2];

	//Connection values
	int user;
	bool syncing = false;
	Solisp::Move *current = NULL;
	Solisp::Game *game = NULL;
};

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