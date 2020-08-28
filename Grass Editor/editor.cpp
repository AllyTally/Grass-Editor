// The main logic for the editor! So basically all of it

#include <memory>
#include <iostream>

#include "editor.h"
#include "map.h"
#include "graphics.h"
#include "input.h"
#include "enums.h"

map::room editor::current_room;
int editor::current_tileset;

void editor::Init() {
    editor::current_tileset = 0;
    editor::current_room = editor::CreateNewRoom();
}

int editor::Logic() {
    if (current_tileset == -1) return MODE_MAPEDITOR;
    if (input::mouse_left_pressed) {
        if ((input::mouse_block_x >= 0 && input::mouse_block_x < editor::current_room.width) && (input::mouse_block_y >= 0 && input::mouse_block_y < editor::current_room.height)) {
            if ((editor::current_room.tiles[input::mouse_block_x + input::mouse_block_y * editor::current_room.width].tileset != editor::current_tileset) ||
                (!editor::current_room.tiles[input::mouse_block_x + input::mouse_block_y * editor::current_room.width].active)) {
                map::tile tile;
                tile.x = 0;
                tile.y = 0;
                tile.solid = true;
                tile.rotation = 0;
                tile.tileset = editor::current_tileset;
                tile.active = true;
                map::SetTile(&editor::current_room, input::mouse_block_x, input::mouse_block_y, tile);
                map::AutotileRadius(&editor::current_room, input::mouse_block_x, input::mouse_block_y);
            }
        }
    }
    if (input::mouse_right_pressed) {
        if ((input::mouse_block_x >= 0 && input::mouse_block_x < editor::current_room.width) && (input::mouse_block_y >= 0 && input::mouse_block_y < editor::current_room.height)) {
            editor::current_room.tiles[input::mouse_block_x + input::mouse_block_y * editor::current_room.width].active = false;
            map::AutotileRadius(&editor::current_room, input::mouse_block_x, input::mouse_block_y);
        }
    }

    if (input::current_keys["T"] == 1) {
        return MODE_TILESETEDITOR;
    }
    return MODE_MAPEDITOR;
}

void editor::Render() {
    //Render texture to screen
    int x = 0;
    int y = 0;
    for (int y = 0; y < current_room.height; y++)
    {
        for (int x = 0; x < current_room.width; x++)
        {
            map::tile current_tile = current_room.tiles[x + y * current_room.width];
            if (!current_tile.active) continue;
            graphics::DrawImagePart(current_room.tilesets[current_tile.tileset].sprite, x * 16, y * 16, current_tile.x * 16, current_tile.y * 16, 16, 16);
        }
    }

    graphics::DrawRectangle(input::mouse_grid_x, input::mouse_grid_y, 16, 16, 255, 0, 255);

    if (current_tileset == -1) {
        graphics::DrawTextBordered(graphics::font, 2, 2, "Current tileset: None (?)", 255, 255, 255);
    }
    else {
        graphics::DrawTextBordered(graphics::font, 2, 2, "Current tileset: " + current_room.tilesets[current_tileset].name, 255, 255, 255);
    }
}

map::room editor::CreateNewRoom() {
    map::room room = map::CreateRoom(20, 15);
    for (int i = 0; i < 10; i++) {
        map::tilesheet grass_tileset = map::ReadTileset("maps/grass.ats");
        if (grass_tileset.successful) {
            room.tilesets.push_back(grass_tileset);
        }
        else {
            std::cout << "The default tileset wasn't able to be loaded!" << std::endl;
            current_tileset = -1;
        }
    }
    return room;
}