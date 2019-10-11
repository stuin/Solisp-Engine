/*
 * Created by Stuart Irwin on 9/25/2019.
 * Godot interface for Interpreter Card
 */

class CardData : public Reference {
	GDCLASS(CardData, Reference);

private:
	Solisp::Card *card;

public:
	CardData() {}

	CardData(Solisp::Card *card) {
		this->card = card;
	}

	int get_slot() {
		return card->getSlot();
	}

	int get_index() {
		return card->getIndex();
	}

	int get_frame() {
		switch(card->getData().suit) {
			case 'H':
				return card->getData().value;
			case 'D':
				return 13 + card->getData().value;
			case 'C':
				return 26 + card->getData().value;
			case 'S':
				return 39 + card->getData().value;
		}
		return 0;
	}

	bool is_hidden() {
		return card->isHidden();
	}
	

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_slot"), &CardData::get_slot);
		ClassDB::bind_method(D_METHOD("get_index"), &CardData::get_index);
		ClassDB::bind_method(D_METHOD("get_frame"), &CardData::get_frame);
		ClassDB::bind_method(D_METHOD("is_hidden"), &CardData::is_hidden);
	}
};