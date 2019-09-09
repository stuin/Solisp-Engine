/*
 * Created by Stuart Irwin on 9/9/2019.
 * Individual card data
 */

struct cardData {
	char value = 'N';
	char suit = 'N';
}

class Card {
private:
	cardData data;
	bool hidden;

	Card *next;

public:
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
		
	}
}