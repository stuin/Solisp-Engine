/*
 * Created by Stuart Irwin on 6/9/2019.
 * Individual slot for cards
 */

class Slot {
private:
	Card *stack;
	Filter *filter;
	bitset<8> tags; //GOAL, INPUT, OUTPUT, SPREAD, HIDDEN, CHANGED, T1, T2
	int count;
	int max;

public:
	bool matches(int count, Card *newCard) {
		if(tags[2])
			return false;

		if(max > 0 && this->count + count > max)
			return false;

		return filter->matches(count, newCard, stack);
	}

}