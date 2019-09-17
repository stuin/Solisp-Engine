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
	void operator+=(const Layout *next) {
		if(this->next == NULL)
			this->next = next;
		else
			*(this->next) += next;
	}
};

class Slot extends Layout {
private:
	bool offScreen = false;
	Stack stack;
	FeatureNode *features;

public:
	Slot(bool horizontal, Feature& feature) : Layout(horizontal) {
		this->type = type;
		this->feature = &feature;
	}
};