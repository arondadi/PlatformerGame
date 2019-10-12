struct Button_State {
	bool is_down;
	bool changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_SPACE,

	BUTTON_COUNT // last item in array
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};