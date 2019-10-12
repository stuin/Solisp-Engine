namespace Solisp {
	class Layout;
	class Slot;
}

/*
 * Created by Stuart Irwin on 28/2/2019.
 * Layout building functions
 */

//Layout for slot arrangement
class Solisp::Layout {
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

class Solisp::Slot : public Solisp::Layout {
private:
	bool offScreen = false;
	int stack;

public:
	Slot(int stack, bool horizontal=false) : Layout(horizontal) {
		this->stack = stack;
	}
};