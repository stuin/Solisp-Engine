#include "../Gameplay/filelist.h"
#include "../Gameplay/game.h"
#include "message.h"

#include <fstream>
#include <streambuf>

//Game state
Solisp::Game game;
unsigned int seed;
std::queue<struct Message> move_queue;

//Connections
std::vector<CActiveSocket *> clients;
std::vector<unsigned int> client_move;

void run_client(CActiveSocket *session, unc player) {
	while(true) {
		//Send move count
		unsigned int size = game.moves.size();
		if(!session->Send((p)("m"), 1))
			break;
		session->Send((p)&size, 4);

		//Send each move
		while(++client_move[player] < size)
			session->Send((p)game.moves.data() + client_move[player], sizeof(Move));

		if(!session->Receive(1))
			break;

		//Receive move from player
		struct Message *msg;
		switch(((char *)session->GetData())[0]) {
			case 'g':
				session->Receive(sizeof(struct Message));
				msg = (struct Message *)session->GetData();
				if(!game.grab(msg->count, msg->slot, player))
					cout << "Rejected grab from player " << (int)player << "\n";
				break;
			case 'p':
				session->Receive(sizeof(struct Message));
				msg = (struct Message *)session->GetData();
				if(!game.place(msg->slot, player))
					cout << "Rejected place from player " << (int)player << "\n";
				break;
			case 'c':
				game.cancel(player);
				break;
		}
	}
	session->Close();
	cout << "Player " << (int)player << " disconnected\n";
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

	//Read rule file
	string ruleFile = "../" + rule_files[game_number];
	std::ifstream t(ruleFile);
	std::string ruleStr((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

	//Prepare other values
	seed = time(NULL);
	cout << "Seed: " << seed << "\n";
	client_move.push_back(0);
	client_move.push_back(0);

	//Build game
	Solisp::Builder *builder = new Solisp::Builder(ruleFile, seed);
	game.setup(builder);
	delete builder;

	//Server
	CPassiveSocket socket;
	socket.Initialize();
	if(!socket.Listen("127.0.0.1", 1234)) {
		cout << "Server error\n";
		return 1;
	}

	cout << "Server started\n";

	while(true) {
		CActiveSocket *session = socket.Accept();
		if(session != NULL) {
			//session->Recive(MAX_PACKET);

			//Notify players
			//for(CActiveSocket *c : clients)
			//	c->Send((const uint8 *)"c", 1);

			//Add client
			clients.push_back(session);
			client_move.push_back(0);
			cout << "Player " << (int)game.players++ << " connected\n";

			//Send rule file
			unsigned int size = ruleStr.length();
			session->Send((p)&size, 4);
			session->Send((p)ruleStr.c_str(), size);

			//Start regular loop
			run_client(session, game.players - 1);
		}
	}

	//Close server and connections
	for(CActiveSocket *c : clients)
		c->Close();
	socket.Close();

	return 0;
}

