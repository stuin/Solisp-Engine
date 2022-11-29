//#include "Button.hpp"

class StatText : public Node {
	Button *button;

	double timer = 0;

public:
	StatText(Button *_button) : Node(INPUT), button(_button) {
		UpdateList::addNode(this);
	}

	string getTime() {
		return std::to_string((int)timer/60) + ":" + std::to_string((int)timer%60);
	}

	void update(double time) {
		timer += time;

		button->setText(getTime());
	}

	void recieveSignal(int id) {
		if(id == STARTGAME)
			timer = 0;
		else if(id == ENDGAME)
			timer = 0;
	}
};