#include "../Gameplay/game.h"

#define _LINUX true
#define MAX_PACKET 4096
#define RULE_CACHE "rules.cache"

#include <clsocket/ActiveSocket.h>
#include <iostream>

using std::string;
using std::cout;
using Solisp::MovePacket;
using Solisp::Move;

Solisp::Game game;
unsigned int move_id;
bool move_updating = false;

void receive_update(CActiveSocket *session) {
	if(move_updating) {
		session->Receive(sizeof(struct MovePacket));
		struct MovePacket *move = (struct MovePacket *) session->GetData();
		cout << "  " << move->id << ": " << move->count << " cards\n";

		if(move->id >= move_id)
			move_updating = false;
	} else {
		session->Receive(1);
		char c = session->GetData()[0];
		switch (c) {
			case 'c':
				cout << "New player connected\n";
				break;
			case 'd':
				cout << "Player disconnected\n";
				break;
			case 'm':
				session->Receive(4);
				memcpy(&move_id, session->GetData(), 4);
				cout << "Receiving moves:\n";
				move_updating = true;
				break;
		}
	}
}

int main(int argc, char const *argv[]) {
	string ip = "127.0.0.1";
	if(argc > 1)
		ip = argv[1];

	CActiveSocket session;
	session.Initialize();
	cout << "Attempting to connect\n";

	if(session.Open(ip.c_str(), 1234)) {
		//Get file size
		session.Receive(4);
		unsigned int rule_size = *((unsigned int *)session.GetData());
		cout << rule_size << "\n";

		//Get full rule file
		session.Receive(rule_size);
		char *rule_file = (char *)malloc(rule_size);
		memcpy(rule_file, (char *)session.GetData(), rule_size);

		//Save rule file to cache
		std::ofstream file(RULE_CACHE);
		file << rule_file;
		file.close();

		//Read first move
		session.Receive(sizeof(struct MovePacket));
		struct MovePacket *move = (struct MovePacket *) session.GetData();
		Move *first = new Move(*move);
		Move *added = first;
		unsigned int seed = first->get_count();

		//Get move id
		session.Receive(1);
		session.Receive(4);
		unsigned int id = *(unsigned int *)session.GetData();

		//Read all other moves
		int count = 1;
		while(move->id < id) {
			session.Receive(sizeof(struct MovePacket));
			move = (struct MovePacket *) session.GetData();
			*added += new Move(*move);
			added = added->get_next();
			++count;
		}

		cout << count << " moves loaded with seed " << seed << "\n";

		//Set game
		Solisp::Builder builder(RULE_CACHE, seed);
		game.setup(&builder, first);
	} else {
		cout << "Connection failed\n";
	}

	session.Close();
	cout << "Closed\n";

	return 0;
}