// This contain the logic for the tileset editor
// Actual tileset loading/handling stuff is in map.cpp

#include "tileset_editor.h"
#include "graphics.h"
#include "map.h"
#include "editor.h"
#include "enums.h"
#include "input.h"


int tileset_editor::Logic() {
    // gaming
    if (input::current_keys["T"] == 1) {
        return MODE_MAPEDITOR;
    }



    return MODE_TILESETEDITOR;
}

void tileset_editor::Render() {
    // gaming

    int x_offset = 0;
    int y_offset = 0;
    for (int i = 0; i < (int)editor::current_room.tilesets.size(); i++) {
        int x_pos = (4 * x_offset);
        int y_pos = 1 + (4 * y_offset);
        DrawBlock(x_pos, y_pos, i);

        int alt_x_pos = 8 + ((4 * x_offset) * 16);
        int alt_y_pos = (1 + (4 * y_offset)) * 16;

        if ((input::mouse_x >= alt_x_pos) && (input::mouse_y >= alt_y_pos) && (input::mouse_x < alt_x_pos + 3 * 16) && (input::mouse_y < alt_y_pos + 3 * 16)) {
            graphics::DrawRectangle(alt_x_pos, alt_y_pos, 48, 48, 255, 0, 255);
        }

        x_offset++;
        if (x_offset > 4) {
            x_offset = 0;
            y_offset++;
        }
    }

    graphics::DrawTextBordered(graphics::font, 2, 2, "welcome to the tile set edito r", 255, 255, 255);
    graphics::DrawTextBordered(graphics::font, 2, 10, "the tile setitor (get it)", 255, 255, 255);
}

void tileset_editor::DrawBlock(int x, int y, int tileset) {
    DrawTile(x  , y  , tileset, map::TILE_TOPLEFT);
    DrawTile(x+1, y  , tileset, map::TILE_TOP);
    DrawTile(x+2, y  , tileset, map::TILE_TOPRIGHT);
    DrawTile(x  , y+1, tileset, map::TILE_MIDDLELEFT);
    DrawTile(x+1, y+1, tileset, map::TILE_MIDDLE);
    DrawTile(x+2, y+1, tileset, map::TILE_MIDDLERIGHT);
    DrawTile(x  , y+2, tileset, map::TILE_BOTTOMLEFT);
    DrawTile(x+1, y+2, tileset, map::TILE_BOTTOM);
    DrawTile(x+2, y+2, tileset, map::TILE_BOTTOMRIGHT);
}

void tileset_editor::DrawTile(int x, int y, int tileset, int index) {
    auto thing = editor::current_room.tilesets[tileset].tileset[index][0];
    graphics::DrawImagePart(editor::current_room.tilesets[tileset].sprite, (x * 16) + 8, y * 16, thing.first * 16, thing.second * 16, 16, 16);
}