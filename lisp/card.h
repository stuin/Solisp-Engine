#pragma once

namespace Solisp {
	struct cardData;
}

struct Solisp::cardData {
	char value = 'N';
	char suit = 'N';

	//Check if normal card matches this filter card
	friend bool operator==(const Solisp::cardData &data, const Solisp::cardData &other) {
		if(data.value != other.value && data.value != 'N')
			return false;

		if(data.suit == 'N' || data.suit == other.suit)
			return true;

		switch(data.suit) {
			case 'R':
				if(other.suit == 'H' || other.suit == 'D')
					return true;
				break;
			case 'B':
				if(other.suit == 'C' || other.suit == 'S')
					return true;
				break;
		}
		return false;
	}
};

char card_face(Solisp::cardData card);