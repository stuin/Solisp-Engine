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
			tags |= (1 << tag);
		else
			tags &= (1 << tag) ^ 0xff;
	}
	bool get_tag(move_tag tag) {
		return tags & (1 << tag);
	}

	//Build new move
	Move() : Move(0, 0, 0, 0, false){
	}
	Move(unc from, unc to, unsigned int count, unc user, bool flip) {
		//Set general move
		this->count = count;
		this->from = from;
		this->to = to;
		this->user = user;

		//Set special tags
		this->tags = 0;
		set_tag(FLIP, flip);
		set_tag(SOFT, user == 1);

		//tags[LOOP] = check_loop(last);
	}

	bool operator==(const Move& other) {
		return count == other.count && from == other.from && to == other.to
			&& tags == other.tags && user == other.user;
	}

};
