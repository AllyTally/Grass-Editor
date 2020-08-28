// The cool map
#include "map.h"
#include "graphics.h"
#include <stdio.h> // Standard IO
#include <SDL.h> // SDL
#include <SDL_image.h> // SDL_Image
#include <string>
#include <vector>
#include <array>
#include <utility>

#include <iostream>
#include <fstream>

map::room map::CreateRoom(int width, int height)
{
    map::room room; // Create a room
    room.width = width; // Set its width...
    room.height = height; // ..and its height.
    room.tiles.reserve(height * width);
    for (auto i = 0; i < height * width; i++) // height * width is the amount of tiles in a room,
    {                                         // so let's create that many
        map::tile tile; // Create a tile
        //tile.x = rand() % 5; // The x and y should both just be
        //tile.y = rand() % 3; // a random value for now
        tile.active = false;
        room.tiles.push_back(tile); // Add the tile to our room
    };
    return room; // Return the room that we just made
};

bool map::SetTile(map::room* room, int x, int y, map::tile tile) {
    if ((x >= 0 && x < room->width) && (y >= 0 && y < room->height)) {
        room->tiles[x + y * room->width] = tile;
        return true;
    }
    return false;
};

map::tilesheet map::ReadTileset(const char* filepath) {
    std::ifstream ATS;
    map::tilesheet read_tileset;
    ATS.open(filepath, std::ios::in | std::ios::binary);
    if (!ATS.is_open()) {
        // We failed to open the file, oops
        std::cout << "Failed to open path '" << filepath << "'. Are you sure the file exists?" << std::endl;
        return read_tileset;
    }
    ATS.seekg(0, std::ios::beg); // Seek to the beginning incase it wasnt there already
    char header_tag[3]; // ATS is three chars long
    ATS.read(header_tag, 3);
    if (header_tag[0] != 'A' || header_tag[1] != 'T' || header_tag[2] != 'S') {
        std::cout << "Invalid file, missing header" << std::endl;
        return read_tileset;
    }

    int file_ver = ATS.get();

    char ch;
    while ((ch = ATS.get()) != '\0') {
        read_tileset.name += ch;
        if (!ATS.good()) {
            std::cout << "Unexpected end of file" << std::endl;
            return read_tileset;
        }
    }

    while ((ch = ATS.get()) != '\0') {
        read_tileset.filepath += ch;
        if (!ATS.good()) {
            std::cout << "Unexpected end of file" << std::endl;
            return read_tileset;
        }
    }

    read_tileset.sprite = graphics::LoadImage(read_tileset.filepath.c_str());
    if (read_tileset.sprite == nullptr) {
        std::cout << "Sprite file not found" << std::endl;
        return read_tileset;
    }

    for (int i = 0; i < (int) read_tileset.tileset.size(); i++) {
        Uint8 size = ATS.get(); // Get the number of items in the vector
        if (!ATS.good()) {
            std::cout << "Unexpected end of file" << std::endl;
            return read_tileset;
        }

        for (int j = 0; j < size; j++) {
            int x = (Uint8) ATS.get();
            if (!ATS.good()) {
                std::cout << "Unexpected end of file" << std::endl;
                return read_tileset;
            }
            int y = (Uint8) ATS.get();
            if (!ATS.good()) {
                std::cout << "Unexpected end of file" << std::endl;
                return read_tileset;
            }
            std::pair<int, int> read = { x, y };
            read_tileset.tileset[i].push_back(read);
        }
    }
    read_tileset.successful = true;
    return read_tileset;
}

bool map::SaveTileset(const char* filepath, map::tilesheet tileset) {
    std::ofstream ATS;
    ATS.open(filepath, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!ATS.is_open()) {
        // We failed to open the file, oops
        std::cout << "Failed to open path '" << filepath << "'. Are you sure the correct directories exist?" << std::endl;
        return false;
    }

    const char* header_string = "ATS";
    Uint8 file_ver = 1;

    ATS.write(header_string, sizeof(header_string) - 1); // Write "ATS" to the file...
    ATS.write((char*)& file_ver, 1);      // ...and also write the file version.

    ATS.write(tileset.name.c_str(), tileset.name.length()); // Write the name to the file

    ATS.write("\0", 1);

    ATS.write(tileset.filepath.c_str(), tileset.filepath.length()); // Write the path to the file
    // Remember--strings are NULL terminated. At first I was planning on storing the length of
    // the string before the actual string, but that just wastes space.
    // Even though we're not exactly aiming to be space-efficient, this just feels better.
    ATS.write("\0", 1);

    for (const auto& tilegroups : tileset.tileset) { // Loop through array of vectors
        Uint8 size = (Uint8) tilegroups.size(); // Get the amount of items in the vector
        ATS.write((char*)& size, sizeof(size)); // Write the number of items in the vector to the file
        for (const auto& pair : tilegroups) { // Loop through the items in the vector
            Uint8 x = pair.first;
            Uint8 y = pair.second;
            ATS.write((char*)& x, sizeof(x)); // Write the x and y to the file
            ATS.write((char*)& y, sizeof(y));
        }
    }
    

    ATS.close(); // Close the file
    return true; // Hey, we saved the file successfully, we should, you know, actually say that
}

bool map::AutoTile(map::room* room, int x, int y) {
    if ((x < 0 || x >= room->width) || (y < 0 || y >= room->height)) {
        return false;
    }
    map::tile current_tile = room->tiles[x + y * room->width];

    bool left = map::CheckAutoTile(room, x - 1, y, current_tile);
    bool top = map::CheckAutoTile(room, x, y - 1, current_tile);
    bool right = map::CheckAutoTile(room, x + 1, y, current_tile);
    bool bottom = map::CheckAutoTile(room, x, y + 1, current_tile);

    bool bottomleft = map::CheckAutoTile(room, x - 1, y + 1, current_tile);
    bool bottomright = map::CheckAutoTile(room, x + 1, y + 1, current_tile);
    bool topleft = map::CheckAutoTile(room, x - 1, y - 1, current_tile);
    bool topright = map::CheckAutoTile(room, x + 1, y - 1, current_tile);



    if (left) { // Left
        if (top) { // Top
            if (right) { // Right
                if (bottom) { // Bottom
                    if (topleft) {
                        if (topright) {
                            if (bottomleft) {
                                if (bottomright) {
                                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_MIDDLE); // FULL
                                }
                                else {
                                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_BOTTOMRIGHTCONNECTOR); // SPECIAL  OWO
                                }
                            }
                            else {
                                if (bottomright) {
                                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_BOTTOMLEFTCONNECTOR);
                                }
                                else {
                                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_MIDDLE);
                                }
                            }
                        }
                        else {
                            if (bottomleft) {
                                if (bottomright) {
                                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_TOPRIGHTCONNECTOR);
                                }
                                else {
                                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_MIDDLE);
                                }
                            }
                            else {
                                map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_MIDDLE);
                            }
                        }
                    }
                    else {
                        if (topright) {
                            if (bottomleft) {
                                if (bottomright) {
                                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_TOPLEFTCONNECTOR);
                                }
                                else {
                                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_MIDDLE);
                                }
                            }
                            else {
                                map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_MIDDLE);
                            }
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_ALL_CONNECT);
                        }
                    }
                }
                else
                {
                    if (topleft) {
                        if (topright) {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_BOTTOM);
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_BOTTOM_TOPRIGHTCONNECTOR);
                        }
                    }
                    else {
                        if (topright) {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_BOTTOM_TOPLEFTCONNECTOR);
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_THREE_BOTTOM);
                        }
                    }
                }
            }
            else
            {
                if (bottom) {
                    if (topleft) {
                        if (bottomleft) {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_MIDDLERIGHT);
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_RIGHT_BOTTOMLEFTCONNECTOR);
                        }
                    }
                    else {
                        if (bottomleft) {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_RIGHT_TOPLEFTCONNECTOR);
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_THREE_RIGHT);
                        }
                    }
                }
                else
                {
                    if (topleft) {
                        map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_BOTTOMRIGHT);
                    }
                    else {
                        map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_BOTTOMRIGHT);
                    }
                }
            }
        }
        else
        {
            if (right) {
                if (bottom) {
                    // only if bottom left and bottom right--change later to allow for 3-way singles and the weird tiles
                    // that moth gave me
                    if (bottomleft) {
                        if (bottomright) {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_TOP);
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_TOP_BOTTOMRIGHTCONNECTOR);
                        }
                    }
                    else {
                        if (bottomright) {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_TOP_BOTTOMLEFTCONNECTOR);
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_THREE_TOP);
                        }
                    }
                }
                else {
                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_HORIZONTAL);
                }
            } 
            else {
                if (bottom) {
                    if (bottomleft) {
                        map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_TOPRIGHT);
                    }
                    else {
                        map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_TOPRIGHT);
                    }
                }
                else {
                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_RIGHT); // also   special   case   owo   ??  ? ?
                }
            }
        }
    }
    else {
        if (top) {
            if (right) {
                if (bottom) {
                    if (topright) {
                        if (bottomright) {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_MIDDLELEFT);
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_LEFT_BOTTOMRIGHTCONNECTOR);
                        }
                    }
                    else {
                        if (bottomright) {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_LEFT_TOPRIGHTCONNECTOR);
                        }
                        else {
                            map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_THREE_LEFT);
                        }
                    }
                }
                else {
                    if (topright) {
                        map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_BOTTOMLEFT);
                    }
                    else {
                        map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_BOTTOMLEFT);
                    }
                }
            }
            else {
                if (bottom) {
                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_VERTICAL);
                }
                else {
                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_DOWN);
                }
            }
        }
        else {
            if (right) {
                if (bottom) {
                    if (bottomright) {
                        map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_TOPLEFT);
                    }
                    else {
                        map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_TOPLEFT);
                    }
                }
                else {
                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_LEFT); // special case
                }
            }
            else {
                if (bottom) {
                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE_UP); // special
                }
                else {
                    map::AdjustTile(room, x, y, current_tile.tileset, map::TILE_SINGLE); // 1x1
                }
            }
        }
    }
    return false;
};

void map::AdjustTile(map::room* room, int x, int y, int tileset, int tilepos) {
    std::vector<std::pair<int, int>> current_vector = room->tilesets[tileset].tileset[tilepos];
    if (current_vector.size() != 0) {
        int random_index = rand() % current_vector.size();
        room->tiles[x + y * room->width].x = current_vector[random_index].first;
        room->tiles[x + y * room->width].y = current_vector[random_index].second;
    }
}   

bool map::CheckAutoTile(map::room* room, int x, int y, map::tile compare_tile) {

    if ((x < 0 || x >= room->width) || (y < 0 || y >= room->height)) {
        return true;
    }
    return (room->tiles[x + y * room->width].solid &&
           room->tiles[x + y * room->width].active &&
           (room->tiles[x + y * room->width].tileset == compare_tile.tileset));
}

void map::AutotileRadius(map::room* room, int tile_x, int tile_y) {
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            map::AutoTile(room, tile_x + x, tile_y + y);
        }
    }
}
