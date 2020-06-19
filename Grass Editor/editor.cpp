// The main logic for the editor! So basically all of it

#include <memory>

#include "editor.h"
#include "map.h"
#include "graphics.h"

map::room editor::current_room;
int editor::current_tileset;

void editor::EditorInit() {
    editor::current_room = editor::CreateEmptyRoom();
    editor::current_tileset = 0;
}

void editor::EditorLogic() {

}

void editor::EditorRender() {
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

    graphics::DrawTextBordered(graphics::font, 2, 2, "Current tileset: " + current_room.tilesets[current_tileset].name, 255, 255, 255);
}

map::room editor::CreateEmptyRoom() {
    map::room room = map::CreateRoom(20, 15);
    map::tilesheet grass_tileset = map::ReadTileset("maps/grass.ats");

    room.tilesets.push_back(grass_tileset);
    return room;
}