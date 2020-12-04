namespace Solisp {
	class Move;
	struct MovePacket;
}

/*
 * Created by Stuart Irwin on 4/9/2019.
 * History of solitare game
 */

static unsigned int max_id = 0;

using unc = unsigned char;
enum move_tag { VALID, FLIP, SETUP, LOOP, SOFT };

struct Solisp::MovePacket {
	unc from;
	unc to;
	unc user;
	unc tags;
	unsigned int count;
	unsigned int id;
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

	void set_tag(move_tag tag, bool value) {
		if(value)
			data.tags |= (1 << tag);
		else
			data.tags &= (1 << tag) ^ 0xff;
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
		data.tags = 0;
		set_tag(VALID, true);
		set_tag(FLIP, flip);
		set_tag(SOFT, user == 1);

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
		if(!get_tag(VALID))
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
		set_tag(VALID, false);
		last->next = next;
		if(next != NULL)
			next->last = last;
		next = NULL;
		last = NULL;
	}

	//Unmark soft move
	void validate() {
		set_tag(SOFT, false);
	}

	void setup() {
		set_tag(SETUP, true);
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
		if(!get_tag(SETUP)) {
			set_tag(VALID, false);
			cout << "Undo move " << data.id << "\n";
			if(data.user == 0 && last != NULL)
				last->undo();
		}
	}

	//Revalidate next move
	void redo(bool first=true) {
		if(get_tag(VALID) && next != NULL)
			next->redo(true);
		else {
			cout << "Redo move " << data.id << "\n";
			set_tag(VALID, true);
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
	bool get_tag(move_tag tag) {
		return data.tags & (1 << tag);
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
