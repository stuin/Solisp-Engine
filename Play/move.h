/*
 * Created by Stuart Irwin on 4/9/2019.
 * History of solitare game
 */

class Move {
private:
	//Card movements
	int count;
	char from;
	char to;
	bitset<4> bits; //player, flip, current, loop

	//List references
	Move *next = NULL;
	Move *last;

public:
	Move(int count, char from, char to, bool player, bool flip, Move *last) {
		this->count = count;
		this->from = from;
		this->to = to;

		bits[0] = player;
		bits[1] = flip;
		bits[2] = true;

		this->last = last;
	}

	//Normal getters
	int getCount();
	char getFrom();
	char getTo();
	bool isPlayer();
	bool isFlip();
	bool isCurrent();
	bool isLoop();
	Move *getNext();
	Move *getLast();

	//Required setters
	void undo();
	void redo();
	void markLoop();
	void addNext(Move *next);
}