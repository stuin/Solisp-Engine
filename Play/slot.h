/*
 * Created by Stuart Irwin on 6/9/2019.
 * Individual slot for cards
 */

class Slot {
private:
	Card *stack;
	Filter filter;
	bitset<8> tags; //GOAL, INPUT, OUTPUT, SPREAD, HIDDEN, CHANGED, T1, T2
	int count;

}