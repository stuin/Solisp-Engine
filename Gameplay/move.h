namespace Solisp {
	class Move;
	struct MovePacket;
}

#include <bitset>

/*
 * Created by Stuart Irwin on 4/9/2019.
 * History of solitare game
 */

#define MOVETAGCOUNT 4
static unsigned int max_id = 0;

using unc = unsigned char;
enum move_tags { FLIP, VALID, SETUP, LOOP, SOFT };

struct Solisp::MovePacket {
	unc from;
	unc to;
	unc user;
	unsigned int count;
	unsigned int id;
	bitset<MOVETAGCOUNT> tags;
};

class Solisp::Move {
private:
	//List references
	struct MovePacket data;
	Move *next = NULL;
	Move *last;

	//Check for circular card movement
	bool check_loop(Move *other) {
		if(data.from == other->get_from() || data.to == other->get_to() ||
				other->get_from() == 0)
			return false;

		if(data.from == other->get_to() && data.to == other->get_from())
			return data.count == other->get_count();

		return check_loop(other->last);
	}

public:
	Move() { }

	Move(struct MovePacket data, Move *last) {
		this->data = data;
		this->last = last;
	}

	//Build new move
	Move(unc from, unc to, unsigned int count, unc user, bool flip, Move *last) {
		//Set general move
		data.count = count;
		data.from = from;
		data.to = to;
		data.user = user;

		//Set special tags
		data.tags[FLIP] = flip;
		data.tags[SETUP] = false;
		data.tags[SOFT] = false;
		data.tags[VALID] = true;

		//Link to previous
		this->last = last;
		data.id = ++max_id;
		//tags[LOOP] = check_loop(last);
	}

	//Recursizely delete backward
	~Move() {
		if(last != NULL)
			delete last;
	}

	//Add new move to history
	void operator+=(Move *other) {
		//Check if state is valid
		if(!data.tags[VALID])
			return;

		//If next move is not current
		if(next == NULL || !next->get_tag(VALID)) {
			clear_forward();
			next = other;
		} else
			*next += other;
	}

	//Skip soft move
	void invalidate() {
		data.tags[VALID] = false;
		last->next = next;
		if(next != NULL)
			next->last = last;
		next = NULL;
		last = NULL;
	}

	//Unmark soft move
	void validate() {
		data.tags[SOFT] = false;
	}

	void setup() {
		data.tags[SETUP] = true;
	}

	//Recursizely delete forward
	void clear_forward() {
		if(next != NULL) {
			next->last = NULL;
			next->clear_forward();
			delete next;
			next = NULL;
		}
	}

	//Set move to invalid
	void undo() {
		if(!data.tags[SETUP]) {
			data.tags[VALID] = false;
			cout << "Undo move " << data.id << "\n";
			if(data.user == 0 && last != NULL)
				last->undo();
		}
	}

	//Revalidate next move
	void redo(bool first=true) {
		if(data.tags[VALID] && next != NULL)
			next->redo(true);
		else {
			cout << "Redo move " << data.id << "\n";
			data.tags[VALID] = true;
			if(next != NULL && next->get_user() == 0)
				next->redo(false);
		}
	}

	//Set actual card count if move-all used
	void correct_count(unsigned int count) {
		if(count < data.count)
			data.count = count;
	}

	struct MovePacket *get_data() {
		return &data;
	}

	//Get tag value
	bool get_tag(int tag) {
		if(tag >= 0 && tag < MOVETAGCOUNT)
			return data.tags[tag];
		return false;
	}

	unc get_user() {
		return data.user;
	}

	//Normal getters
	unc get_from() {
		return data.from;
	}

	unc get_to() {
		return data.to;
	}

	unsigned int get_count() {
		return data.count;
	}

	unsigned int get_id() {
		return data.id;
	}

	Move *get_next() {
		return next;
	}

	Move *get_last() {
		return last;
	}
};
