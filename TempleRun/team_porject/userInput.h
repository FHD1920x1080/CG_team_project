#pragma once

class UserInput {
public:
	bool left_down = false;
	bool A_Down, S_Down, W_Down, D_Down, C_Down, Space_Down;
	float cursor_x = 0.0, cursor_y = 0.0;
	float cursor_x1 = 0.0, cursor_y1 = 0.0;
	float cursor_x2 = 0.0, cursor_y2 = 0.0;
};

UserInput userInput;