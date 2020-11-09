#include "../Gameplay/filelist.h"
#include "../Gameplay/game.h"

#include <brynet/net/EventLoop.hpp>
#include <brynet/net/TcpService.hpp>
#include <brynet/net/wrapper/ServiceBuilder.hpp>

#include <iostream>
#include <queue>

using std::cout;
using std::string;
using Solisp::MovePacket;
using Solisp::Hand;
using namespace brynet;
using namespace brynet::net;

struct SETUP {
	unc players = 0;
	unsigned int seed;
};

Solisp::Game game;
struct SETUP setup;
std::queue<struct Hand> move_queue;

std::vector<TcpConnection::Ptr> clients;
std::vector<Solisp::Move *> client_move;

void update_client(const TcpConnection::Ptr& session, unc player) {
	//Send move ends
	unsigned int id = game.get_current()->get_id();
	char data[sizeof(struct MovePacket)];
	session->send("m" + id, 5);

	//Send each move
	Solisp::Move *move = client_move[player];
	while(move->get_id() < id) {
		move = move->get_next();
		memcpy(data, move->get_data(), sizeof(struct MovePacket));
		session->send(data, sizeof(struct MovePacket));
	}
	client_move[player] = move;
}

int main(int argc, char const *argv[]) {
	int game_number;
	if(argc > 1) {
		cout << "Game number = " << argv[1] << "\n";
		game_number = argv[1][0] - '0';
	} else {
		cout << "Game number: ";
		std::cin >> game_number;
	}

	string ruleStr = rule_files[game_number];
	cout << ruleStr << "\n";

	//Build setup packet
	setup.seed = time(NULL);

	//Build game
	Solisp::Builder *builder = new Solisp::Builder(rule_files[game_number], setup.seed);
	game.setup(builder);
	game.update();
	delete builder;
	game.players = 0;

	//Server
	int port = atoi("23");
	auto mainLoop = std::make_shared<EventLoop>();
	auto service = TcpService::Create();

	auto listenThread = ListenThread::Create(false, "0.0.0.0", port,
			[mainLoop, service, ruleStr](TcpSocket::Ptr socket) {
        socket->setNodelay();
        socket->setSendSize(32 * 1024);
        socket->setRecvSize(32 * 1024);

		auto enterCallback = [mainLoop, ruleStr](const TcpConnection::Ptr& session) {
			mainLoop->runAsyncFunctor([session, ruleStr]() {
				//Notify players
				for(TcpConnection::Ptr client : clients)
					client->send("c", 1);

				//Add client
				setup.players = ++game.players;
				clients.push_back(session);
				cout << "Player " << setup.players << " connected\n";

				//Send setup packet
				char data[sizeof(struct SETUP) + ruleStr.length()];
				memcpy(data, &setup, sizeof setup);
				memcpy(data + sizeof setup, ruleStr.data(), ruleStr.length());
				session->send(data, sizeof(struct SETUP) + ruleStr.length());
			});

			//Set up recieving packets
			session->setDataCallback([session, mainLoop](const char* buffer, size_t len) {
				struct Hand *hand = (struct Hand*) buffer;

				mainLoop->runAsyncFunctor([session, hand]() {
					cout << "Move from player " << hand->tested << "\n";
					move_queue.push(*hand);
					update_client(session, hand->tested);
				});
				return len;
			});

			//On disconnect
			session->setDisConnectCallback([mainLoop](const TcpConnection::Ptr& session) {
	            (void)session;
	            //setup.players = --game.players;

	            //Notify players
	            mainLoop->runAsyncFunctor([session]() {
	            	cout << "Player disconnected\n";
	            	for(auto it = clients.begin(); it != clients.end(); ++it) {
				        if(*it == session)
				            clients.erase(it);
				        else
				        	(*it)->send("d", 1);
				    }
		        });
	        });
		};

		service->addTcpConnection(std::move(socket),
            brynet::net::AddSocketOption::AddEnterCallback(enterCallback),
            brynet::net::AddSocketOption::WithMaxRecvBufferSize(1024 * 1024));
	});

	listenThread->startListen();
	service->startWorkerThread(port);
	cout << "Server started\n";

	while(true) {
		mainLoop->loop(10);
	}

	return 0;
}

