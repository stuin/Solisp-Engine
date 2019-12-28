/*
 * Created by Stuart Irwin on 9/25/2019.
 * Godot interface for Interpreter Card
 */

class CardData : public Reference {
	GDCLASS(CardData, Reference);

private:
	Solisp::Card *card;

public:
	CardData() {
		this->card = new Solisp::Card(1, 'S');
	}

	CardData(Solisp::Card *card) {
		this->card = card;
	}

	int get_slot() {
		return card->get_slot();
	}

	int get_index() {
		return card->get_index();
	}

	int get_frame() {
		switch(card->get_data().suit) {
			case 'C':
				return card->get_data().value - 1;
			case 'D':
				return 12 + card->get_data().value;
			case 'H':
				return 25 + card->get_data().value;
			case 'S':
				return 38 + card->get_data().value;
		}
		return 0;
	}

	bool is_hidden() {
		return card->is_hidden();
	}
	

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_slot"), &CardData::get_slot);
		ClassDB::bind_method(D_METHOD("get_index"), &CardData::get_index);
		ClassDB::bind_method(D_METHOD("get_frame"), &CardData::get_frame);
		ClassDB::bind_method(D_METHOD("is_hidden"), &CardData::is_hidden);
	}
};