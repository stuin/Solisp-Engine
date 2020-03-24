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
		if(get_offset_x() < 0) {
			if(stack->get_max() != -1)
				return stack->x + stack->get_max() - 2;
			return stack->x + 6;
		}
		return stack->x;
	}

	int get_y() {
		return stack->y;
	}

	int get_offset_x() {
		if(stack->get_tag(SPREAD) && stack->get_tag(SPREAD_HORIZONTAL))
			return stack->get_tag(SPREAD_REVERSE) ? -1 : 1;
		return 0;
	}

	int get_offset_y() {
		if(stack->get_tag(SPREAD) && !stack->get_tag(SPREAD_HORIZONTAL))
			return stack->get_tag(SPREAD_REVERSE) ? -1 : 1;
		return 0;
	}

	bool is_button() {
		return stack->get_tag(BUTTON);
	}

	String test_list() {
		if(stack->get_card() != NULL)
			return String(stack->get_card()->print_stack().c_str());
		return String();
	}

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_x"), &SlotData::get_x);
		ClassDB::bind_method(D_METHOD("get_y"), &SlotData::get_y);
		ClassDB::bind_method(D_METHOD("get_offset_x"), &SlotData::get_offset_x);
		ClassDB::bind_method(D_METHOD("get_offset_y"), &SlotData::get_offset_y);
		ClassDB::bind_method(D_METHOD("is_button"), &SlotData::is_button);
		ClassDB::bind_method(D_METHOD("test_list"), &SlotData::test_list);
	}
};