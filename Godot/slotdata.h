/*
 * Created by Stuart Irwin on 9/25/2019.
 * Godot interface for Gameplay Slot
 */

class SlotData : public Reference {
	GDCLASS(SlotData, Reference);

private:
	Solisp::Stack *stack;

public:
	SlotData() {
		stack = NULL;
	}

	SlotData(Solisp::Stack *stack) {
		this->stack = stack;
	}

	int get_x() {
		return stack->x;
	}

	int get_y() {
		return stack->y;
	}

	int get_offset_x() {
		if(stack->get_tag(SPREAD) && stack->get_tag(SPREAD_HORIZONTAL))
			return 1;
		return 0;
	}

	int get_offset_y() {
		if(stack->get_tag(SPREAD) && !stack->get_tag(SPREAD_HORIZONTAL))
			return 1;
		return 0;
	}
	

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_x"), &SlotData::get_x);
		ClassDB::bind_method(D_METHOD("get_y"), &SlotData::get_y);
		ClassDB::bind_method(D_METHOD("get_offset_x"), &SlotData::get_offset_x);
		ClassDB::bind_method(D_METHOD("get_offset_y"), &SlotData::get_offset_y);
	}
};