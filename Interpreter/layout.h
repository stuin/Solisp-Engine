#include "stack.h"

/*
 * Created by Stuart Irwin on 28/2/2019.
 * Layout building functions
 */

//Layout for slot arrangement
class Layout {
private:
	bool horizontal;
	Layout *inside = NULL;
	Layout *next = NULL;

public:
	//Build layout
	Layout(bool horizontal) {
		this->horizontal = horizontal;
	}

	//Insert internal node
	void operator<<(const Layout *child) {
		if(inside == NULL)
			inside = next;
		else
			*inside += next;
	}

	//Add next node
	void operator+=(Layout *next) {
		if(this->next == NULL)
			this->next = next;
		else
			*(this->next) += next;
	}
};

class Slot : public Layout {
private:
	bool offScreen = false;
	Stack *stack;

public:
	Slot(bool spread, bool goal=false, bool horizontal=false) : Layout(horizontal) {
		bitset<STACKTAGCOUNT> tags;
		tags[0] = goal;
		tags[3] = spread;

		stack = new Stack(tags);
	}
};