// Container for input

#include "input.h"

#include <unordered_map>
#include <string>

int  input::mouse_x;
int  input::mouse_y;
int  input::mouse_grid_x;
int  input::mouse_grid_y;
int  input::mouse_block_x;
int  input::mouse_block_y;
bool input::mouse_left_down;
bool input::mouse_left_pressed;
bool input::mouse_left_up;
bool input::mouse_right_down;
bool input::mouse_right_pressed;
bool input::mouse_right_up;
std::unordered_map<std::string, int> input::current_keys;