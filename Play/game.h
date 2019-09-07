#include <vector>
#include <queue>

#include "structures.h"

/*
 * Created by Stuart Irwin on 29/6/2019.
 * Game rules and management.
 */

class Game {
private:
	//Current game state
	Slot slots[];
	Move *current;
	Feature functions;
	bool ended;

	void move();
	void update();

public:
	//Player actions
	bool grab(int num, char from);
	bool place(char to);

	void undo();
	void redo();

	//Check game state
	bool ended();
}
