/*
    GRASS EDITOR
     Ally  2020

    A map editor for the work-in-progress game "Grass"
    Fun fact: this is my first real C++ project, so I
    apologize in advance for the bad code
*/

#include <stdio.h> // Standard IO
#include <cstdlib> // Standard Library
#include <string> // Strings
#include <SDL.h> // SDL
#include <SDL_image.h> // SDL_Image
#include <SDL_ttf.h> // SDL_ttf
#include <iostream>

#include <vector>
#include <array>
#include <utility>

#include "map.h"
#include "graphics.h"
#include "editor.h"
#include "enums.h"

// Screen dimension constants
const int   SCREEN_WIDTH  = 320;
const int   SCREEN_HEIGHT = 240;
const int   SCREEN_SCALE  = 2;
const char* WINDOW_TITLE  = "Grass Editor";

// The state of the program
int state = MODE_MAPEDITOR;

// The window we'll be rendering to
SDL_Window*   g_window = NULL;
SDL_Surface*  g_surface = NULL;
SDL_Renderer* g_renderer = NULL;

int main(int argc, char* args[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    // Create the window
    // m_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIDDEN);

    g_window = SDL_CreateWindow(
        WINDOW_TITLE, // window title
        SDL_WINDOWPOS_UNDEFINED, // initial x position
        SDL_WINDOWPOS_UNDEFINED, // initial y position
        SCREEN_WIDTH, // width, in pixels
        SCREEN_HEIGHT, // height, in pixels
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE // flags - see below
    );
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (g_window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return EXIT_FAILURE;
    }
   
    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // Set up the renderer
    SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0xFF);
    // Resize the window
    SDL_RenderSetLogicalSize(g_renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetIntegerScale(g_renderer, SDL_FALSE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetWindowSize(g_window, SCREEN_WIDTH * SCREEN_SCALE, SCREEN_HEIGHT * SCREEN_SCALE);

    SDL_SetWindowPosition(g_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // Show the window

    SDL_ShowWindow(g_window);

    // Get the window's surface
    g_surface = SDL_GetWindowSurface(g_window);

    // Make it black!
    SDL_FillRect(g_surface, NULL, SDL_MapRGB(g_surface->format, 0x00, 0x00, 0x00));
    
    // Initialize our namespaces!
    graphics::Init();
    editor::EditorInit();


    bool quit = false;
    SDL_Event e;

    int mouse_x = 0;
    int mouse_y = 0;
    int mouse_grid_x = 0;
    int mouse_grid_y = 0;
    int mouse_block_x = 0;
    int mouse_block_y = 0;

    bool mouse_left_down = false;
    bool mouse_left_pressed = false;
    bool mouse_left_up = false;
    bool mouse_right_down = false;
    bool mouse_right_pressed = false;
    bool mouse_right_up = false;

    while (!quit)
    {
        mouse_left_down = false;
        mouse_left_up = false;
        mouse_right_down = false;
        mouse_right_up = false;
        while (SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                mouse_x = e.motion.x;
                mouse_y = e.motion.y;
                mouse_block_x = mouse_x / 16;
                mouse_block_y = mouse_y / 16;
                mouse_grid_x = mouse_block_x * 16;
                mouse_grid_y = mouse_block_y * 16;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_left_down = true;
                    mouse_left_pressed = true;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    mouse_right_down = true;
                    mouse_right_pressed = true;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_left_up = true;
                    mouse_left_pressed = false;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    mouse_right_up = true;
                    mouse_right_pressed = false;
                }
            }
            //User presses a key
            else if (e.type == SDL_KEYDOWN)
            {
                //Select surfaces based on key press
                switch (e.key.keysym.sym)
                {
                    case SDLK_s:
                    {
                        std::array<std::vector<std::pair<int, int>>, map::TILE_ENUM_LENGTH> ts{};
                        ts[map::TILE_TOPLEFT]              = { {0,0} };
                        ts[map::TILE_TOP]                  = { {1,0} };
                        ts[map::TILE_TOPRIGHT]             = { {2,0} };
                        ts[map::TILE_MIDDLELEFT]           = { {0,1} };
                        ts[map::TILE_MIDDLE]               = { {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {1,1}, {4,0} };
                        ts[map::TILE_MIDDLERIGHT]          = { {2,1} };
                        ts[map::TILE_BOTTOMLEFT]           = { {0,2} };
                        ts[map::TILE_BOTTOM]               = { {1,2} };
                        ts[map::TILE_BOTTOMRIGHT]          = { {2,2} };

                        ts[map::TILE_BOTTOMLEFTCONNECTOR]  = { {4,1} };
                        ts[map::TILE_BOTTOMRIGHTCONNECTOR] = { {3,1} };
                        ts[map::TILE_TOPLEFTCONNECTOR]     = { {4,2} };
                        ts[map::TILE_TOPRIGHTCONNECTOR]    = { {3,2} };

                        ts[map::TILE_SINGLE]               = { {3,0} };

                        ts[map::TILE_SINGLE_LEFT]          = { {0,3} };
                        ts[map::TILE_SINGLE_HORIZONTAL]    = { {1,3} };
                        ts[map::TILE_SINGLE_RIGHT]         = { {2,3} };
                        ts[map::TILE_SINGLE_UP]            = { {0,4} };
                        ts[map::TILE_SINGLE_VERTICAL]      = { {1,4} };
                        ts[map::TILE_SINGLE_DOWN]          = { {2,4} };

                        ts[map::TILE_SINGLE_TOPLEFT]       = { {3,3} };
                        ts[map::TILE_SINGLE_TOPRIGHT]      = { {4,3} };
                        ts[map::TILE_SINGLE_BOTTOMLEFT]    = { {3,4} };
                        ts[map::TILE_SINGLE_BOTTOMRIGHT]   = { {4,4} };

                        ts[map::TILE_SINGLE_ALL_CONNECT]   = { {5,0} };
                        ts[map::TILE_SINGLE_THREE_BOTTOM]  = { {5,1} };
                        ts[map::TILE_SINGLE_THREE_TOP]     = { {5,3} };
                        ts[map::TILE_SINGLE_THREE_LEFT]    = { {5,2} };
                        ts[map::TILE_SINGLE_THREE_RIGHT]   = { {5,4} };

                        ts[map::TILE_TOP_BOTTOMRIGHTCONNECTOR] = { {6,0} };
                        ts[map::TILE_TOP_BOTTOMLEFTCONNECTOR]  = { {7,0} };
                        ts[map::TILE_BOTTOM_TOPRIGHTCONNECTOR] = { {6,1} };
                        ts[map::TILE_BOTTOM_TOPLEFTCONNECTOR]  = { {7,1} };

                        ts[map::TILE_LEFT_BOTTOMRIGHTCONNECTOR]       = { {6,2} };
                        ts[map::TILE_LEFT_TOPRIGHTCONNECTOR]          = { {6,3} };
                        ts[map::TILE_RIGHT_BOTTOMLEFTCONNECTOR]       = { {7,2} };
                        ts[map::TILE_RIGHT_TOPLEFTCONNECTOR]          = { {7,3} };

                        map::tilesheet a;
                        a.filepath = "grass.png";
                        a.name = "Grass";
                        a.tileset = ts;
                        bool saved = map::SaveTileset("maps/test.ats", a);
                        //std::cout << saved << std::endl;
                        break;
                    }
                    case SDLK_o:
                        map::tilesheet a = map::ReadTileset("maps/test.ats");
                        bool saved = map::SaveTileset("maps/test2.ats", a);
                        break;
                }
            }
        }
        if (quit) break;

        if (mouse_left_pressed) {
            if ((mouse_block_x >= 0 && mouse_block_x < editor::current_room.width) && (mouse_block_y >= 0 && mouse_block_y < editor::current_room.height)) {
                if ((editor::current_room.tiles[mouse_block_x + mouse_block_y * editor::current_room.width].tileset != editor::current_tileset) ||
                    (!editor::current_room.tiles[mouse_block_x + mouse_block_y * editor::current_room.width].active)) {
                    map::tile tile;
                    tile.x = 0;
                    tile.y = 0;
                    tile.solid = true;
                    tile.rotation = 0;
                    tile.tileset = editor::current_tileset;
                    tile.active = true;
                    map::SetTile(&editor::current_room, mouse_block_x, mouse_block_y, tile);
                    map::AutotileRadius(&editor::current_room, mouse_block_x, mouse_block_y);
                }
            }
        }
        if (mouse_right_pressed) {
            if ((mouse_block_x >= 0 && mouse_block_x < editor::current_room.width) && (mouse_block_y >= 0 && mouse_block_y < editor::current_room.height)) {
                editor::current_room.tiles[mouse_block_x + mouse_block_y * editor::current_room.width].active = false;
                map::AutotileRadius(&editor::current_room, mouse_block_x, mouse_block_y);
            }
        }

        // Logic state machine!
        switch (state) {
            case MODE_MAPEDITOR:
                editor::EditorLogic();
                break;
        };

        //Clear screen
        SDL_RenderClear(g_renderer);

        // Rendering state machine!
        switch (state) {
        case MODE_MAPEDITOR:
            editor::EditorRender();
            break;
        };

        //Update screen
        SDL_RenderPresent(g_renderer);
    }
    SDL_FreeSurface(g_surface);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    g_surface = NULL;
    g_renderer = NULL;
    g_window = NULL;

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}