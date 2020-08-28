#pragma once

#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>
#include <string>

namespace input
{
    extern std::unordered_map<std::string, int> current_keys;

    extern int mouse_x;
    extern int mouse_y;
    extern int mouse_grid_x;
    extern int mouse_grid_y;
    extern int mouse_block_x;
    extern int mouse_block_y;
    extern bool mouse_left_down;
    extern bool mouse_left_pressed;
    extern bool mouse_left_up;
    extern bool mouse_right_down;
    extern bool mouse_right_pressed;
    extern bool mouse_right_up;

};

#endif