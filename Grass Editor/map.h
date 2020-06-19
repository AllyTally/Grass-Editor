#pragma once

#ifndef MAP_H
#define MAP_H

#include <SDL.h> // SDL
#include <SDL_image.h> // SDL_Image
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <utility>

extern SDL_Window* g_window;
extern SDL_Surface* g_surface;
extern SDL_Renderer* g_renderer;

namespace map
{
    enum tilemap {
        TILE_TOPLEFT             ,
        TILE_TOP                 ,
        TILE_TOPRIGHT            ,
        TILE_MIDDLELEFT          ,
        TILE_MIDDLE              ,
        TILE_MIDDLERIGHT         ,
        TILE_BOTTOMLEFT          ,
        TILE_BOTTOM              ,
        TILE_BOTTOMRIGHT         ,
        TILE_BOTTOMLEFTCONNECTOR ,
        TILE_BOTTOMRIGHTCONNECTOR,
        TILE_TOPLEFTCONNECTOR    ,
        TILE_TOPRIGHTCONNECTOR   ,
        TILE_SINGLE              ,

        TILE_SINGLE_LEFT         ,
        TILE_SINGLE_HORIZONTAL   ,
        TILE_SINGLE_RIGHT        ,

        TILE_SINGLE_UP           ,
        TILE_SINGLE_VERTICAL     ,
        TILE_SINGLE_DOWN         ,

        TILE_SINGLE_TOPLEFT      ,
        TILE_SINGLE_TOPRIGHT     ,
        TILE_SINGLE_BOTTOMLEFT   ,
        TILE_SINGLE_BOTTOMRIGHT  ,

        TILE_SINGLE_ALL_CONNECT  ,
        TILE_SINGLE_THREE_BOTTOM ,
        TILE_SINGLE_THREE_TOP    ,
        TILE_SINGLE_THREE_LEFT   ,
        TILE_SINGLE_THREE_RIGHT  ,

        TILE_TOP_BOTTOMRIGHTCONNECTOR,
        TILE_TOP_BOTTOMLEFTCONNECTOR,
        TILE_BOTTOM_TOPRIGHTCONNECTOR,
        TILE_BOTTOM_TOPLEFTCONNECTOR,

        TILE_LEFT_TOPRIGHTCONNECTOR,
        TILE_LEFT_BOTTOMRIGHTCONNECTOR,
        TILE_RIGHT_TOPLEFTCONNECTOR,
        TILE_RIGHT_BOTTOMLEFTCONNECTOR,

        TILE_ENUM_LENGTH
    };
    struct tilesheet {
        std::string filepath;
        std::string name;
        SDL_Texture* sprite;
        //int width;
        //int height;
        std::array<std::vector<std::pair<int, int>>,TILE_ENUM_LENGTH> tileset;
    };
    struct tile {
        int tileset = 0;
        int x = 0;
        int y = 0;
        bool solid = false;
        int rotation = 0;
        bool active = true;
    };
    struct room {
        std::vector<tilesheet> tilesets;
        std::vector<tile> tiles;
        int width = 40;
        int height = 30;
    };

    room CreateRoom(int width, int height);

    bool SetTile(room* room, int x, int y, tile tile);
    map::tilesheet ReadTileset(const char* filepath);
    bool SaveTileset(const char* filepath, tilesheet tileset);
    bool AutoTile(map::room* room, int x, int y);
    void AdjustTile(map::room* room, int x, int y, int tileset, int tilepos);
    bool CheckAutoTile(map::room* room, int x, int y, map::tile compare_tile);
    void AutotileRadius(map::room* room, int tile_x, int tile_y);
    //bool SetTile(int x, int y, int tile_x, int tile_y);
};

#endif