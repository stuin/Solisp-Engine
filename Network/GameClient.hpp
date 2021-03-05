#include "message.h"

class GameClient : public Solisp::GameInterface {
private:
	//Incoming game state
	unsigned int server = 0;
	unsigned int move_count = 0;
	bool move_updating = false;

	//Outgoing game state
	std::queue<Message> move_queue;
	Game *game;
	unc user = 2;

	//Client
	CActiveSocket session;
	std::thread *networking;

	void run_updates() {
		while(true) {
			while(move_queue.empty())
				std::this_thread::sleep_for(std::chrono::seconds(1));

			//Send move to server
			cout << "Sending move\n";
			session.Send((p)&move_queue.front().type, 1);
			session.Send((p)&move_queue.front(), sizeof(Message));
			move_queue.pop();

			if(!session.Receive(1))
				break;

			//Get message from server
			switch (session.GetData()[0]) {
				case 'c':
					cout << "New player connected\n";
					break;
				case 'd':
					cout << "Player disconnected\n";
					break;
				case 'm':
					session.Receive(4);
					move_count = *(unsigned int *)session.GetData();
					while(++server < move_count) {
						session.Receive(sizeof(Move));
						game->apply(*(Move *)session.GetData(), server++);
					}
					reloadAll();
					break;
			}
		}
		session.Close();
		cout << "Lost connection to server\n";
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

			//Get move id
			session.Receive(1);
			session.Receive(4);
			move_count = *(unsigned int *)session.GetData();

			//Get first move
			session.Receive(sizeof(Move));
			game->apply(*(Move*) session.GetData());
			unsigned int seed = game->moves[0].count;

			//Read all other moves
			while(++server < move_count) {
				session.Receive(sizeof(Move));
				game->apply(*(Move*) session.GetData());
			}

			cout << server << " moves loaded with seed " << seed << "\n";

			//Set game
			Solisp::Builder builder(RULE_CACHE, seed);
			game->setup(&builder, true);

			cout << "Multiplayer game started\n";
			networking = new std::thread([this] { run_updates(); });
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
		move_queue.push({'g', from, num});
		return game->grab(num, from, user);
	}
	bool test(unc to, unc user) {
		return game->test(to, user);
	}
	bool place(unc to, unc user) {
		move_queue.push({'p', to, 0});
		return game->test(to, user);
	}
	void cancel(unc user) {
		return game->cancel(user);
	}
};