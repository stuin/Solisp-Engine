namespace Solisp {
	class Card;
	struct cardData;
}

/*
 * Created by Stuart Irwin on 9/9/2019.
 * Individual card data
 */

struct Solisp::cardData {
	char value = 'N';
	char suit = 'N';
};

class Solisp::Card {
private:
	cardData data;
	bool hidden;

	Card *next;
	int slot = 0;

public:
	//Constructors
	Card(cardData data) {
		this->data = data;
	}
	Card(char value, char suit) {
		data.value = value;
		data.suit = suit;
	}

	//General getters
	cardData getData() {
		return data;
	}
	Card *getNext() {
		return next;
	}
	int getSlot() {
		return slot;
	}

	//Check what side is up
	bool isHidden() {
		return hidden;
	}

	//Check if normal card matches this filter card
	bool matches(cardData other) {
		if(data.value != other.value && data.value != 'N')
			return false;

		if(data.suit == 'N' || data.suit == other.suit)
			return true;

		switch(data.suit) {
			case 'R':
				if(other.suit == 'H' || other.suit == 'D')
					return true;
			case 'B':
				if(other.suit == 'C' || other.suit == 'S')
					return true;
		}
		return false;
	}

	//Flip card
	void flip() {
		hidden = !hidden;
	}

	//General setters
	void setNext(Card *next) {
		this->next = next;
	}
	void setSlot(int slot) {
		this->slot = slot;
	}

	//Card setup functions
	Card(char start, char end, char suit);
	Card *reverse(Card *last=NULL);
	Card *withSuit(char suit);
	Filter *fourSuit();
	Filter *alternating();
	Card *alternating(bool black);
};