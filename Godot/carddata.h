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

	int getFrame() {
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

	bool isHidden() {
		return card->isHidden();
	}
	

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("getFrame"), &CardData::getFrame);
		ClassDB::bind_method(D_METHOD("isHidden"), &CardData::isHidden);
	}
};