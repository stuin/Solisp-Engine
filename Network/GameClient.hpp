#define _LINUX true
#define MAX_PACKET 4096
#define RULE_CACHE "rules.cache"

#include <clsocket/ActiveSocket.h>
#include <iostream>
#include <queue>

using std::string;
using std::cout;
using Solisp::MovePacket;
using Solisp::Move;
using Solisp::Game;

class GameClient : public Solisp::GameInterface {
private:
	//Incoming game state
	Move *server;
	unsigned int move_id;
	bool move_updating = false;

	//Outgoing game state
	std::queue<Solisp::Hand> move_queue;
	Game *game;
	unc user = 2;

	//Client
	CActiveSocket session;

	void receive_update() {
		if(move_updating) {
			session.Receive(sizeof(struct MovePacket));
			struct MovePacket *data = (struct MovePacket *) session.GetData();
			*server += new Move(*data, true);

			if(data->id >= move_id)
				move_updating = false;
		} else {
			session.Receive(1);
			char c = session.GetData()[0];
			switch (c) {
				case 'c':
					cout << "New player connected\n";
					break;
				case 'd':
					cout << "Player disconnected\n";
					break;
				case 'm':
					session.Receive(4);
					memcpy(&move_id, session.GetData(), 4);
					cout << "Receiving moves:\n";
					move_updating = true;
					break;
			}
		}
	}
public:
	~GameClient() {
		cout << "Closing connection\n";
		session.Close();
	}

	bool connect(string ip, Game *game) {
		this->game = game;
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
			struct MovePacket *data = (struct MovePacket *) session.GetData();
			Move *first = new Move(*data);
			Move *added = first;
			unsigned int seed = first->get_count();

			//Get move id
			session.Receive(1);
			session.Receive(4);
			unsigned int id = *(unsigned int *)session.GetData();

			//Read all other moves
			int count = 1;
			while(data->id < id) {
				session.Receive(sizeof(struct MovePacket));
				data = (struct MovePacket *) session.GetData();
				*added += new Move(*data, true);
				added = added->get_next();
				++count;
			}
			server = added;

			cout << count << " moves loaded with seed " << seed << "\n";

			//Set game
			Solisp::Builder builder(RULE_CACHE, seed);
			game->setup(&builder, first);
			game->update();

			cout << "Multiplayer game started\n";
			return true;
		} else {
			cout << "Connection failed\n";
			return false;
		}
	}

	unc get_user() {
		return user;
	}

	bool grab(unsigned int num, unc from, unc user) {
		return game->grab(num, from, user);
	}
	bool test(unc to, unc user) {
		return game->test(to, user);
	}
	bool place(unc to, unc user) {
		move_queue.push(game->get_hand(user));
		return game->place(to, user);
	}
	void cancel(unc user) {
		return game->cancel(user);
	}
};