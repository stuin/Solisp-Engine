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

	void setCard(Solisp::Card *card) {
		this->card = card;
	}

	int getFace() {
		return card->getData().value;
	}

	int getSuit() {
		return card->getData().suit;
	}

	bool isHidden() {
		return card->isHidden();
	}
	

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("getFace"), &CardData::getFace);
		ClassDB::bind_method(D_METHOD("getSuit"), &CardData::getSuit);
		ClassDB::bind_method(D_METHOD("isHidden"), &CardData::isHidden);
	}
};