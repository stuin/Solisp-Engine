namespace Solisp {
	struct Move;
}

/*
 * Created by Stuart Irwin on 4/9/2019.
 * History of solitare game
 */

using unc = unsigned char;
enum move_tag { FLIP, LOOP, SOFT, SETUP };

struct Solisp::Move {
	unc from;
	unc to;
	unc user;
	unsigned char tags;
	unsigned int count;

	//bitwise tag values
	void set_tag(move_tag tag, bool value) {
		if(value)
			data.tags |= (1 << tag);
		else
			data.tags &= (1 << tag) ^ 0xff;
	}
	bool get_tag(move_tag tag) {
		return data.tags & (1 << tag);
	}

	//Build new move
	Move(unc from, unc to, unsigned int count, unc user, bool flip) {
		//Set general move
		count = count;
		from = from;
		to = to;
		user = user;

		//Set special tags
		tags = 0;
		set_tag(FLIP, flip);
		set_tag(SOFT, user == 1);

		//tags[LOOP] = check_loop(last);
	}

};

class Solisp::Move {
private:
	//List references
	struct MovePacket data;
	Move *next = NULL;
	Move *last = NULL;

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

	

	//Recursizely delete backward
	~Move() {
		if(last != NULL)
			delete last;
	}

	//Add new move to history
	void operator+=(Move *other) {
		//Find last valid state
		Move *current = this;
		while(!current->get_tag(VALID) && current->last != NULL)
			current = current->last;

		//Find most recent valid move
		while(current->next != NULL && current->next->get_tag(VALID) &&
			(!other->get_tag(SERVER) || current->next->get_tag(SERVER)))
			current = current->next;

		//Add new move
		current->clear_forward();
		current->next = other;
		other->last = current;
	}

	//Skip soft move
	void soft_invalidate() {
		set_tag(VALID, false);
		last->next = next;
		if(next != NULL)
			next->last = last;
		next = NULL;
		last = NULL;
	}

	//Unmark soft move
	void soft_validate() {
		set_tag(SOFT, false);
	}



	//Recursizely delete forward
	void clear_forward() {
		if(next != NULL) {
			if(next->get_tag(VALID)) {
				next->set_tag(VALID, false);
				next->clear_forward();
				next = NULL;
			} else {
				next->last = NULL;
				next->clear_forward();
				delete next;
				next = NULL;
			}
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
		
	}

	struct MovePacket *get_data() {
		return &data;
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
