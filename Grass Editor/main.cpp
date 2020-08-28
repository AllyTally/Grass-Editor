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
#include "enums.h"
#include "input.h"

// Gamestates
#include "editor.h"
#include "tileset_editor.h"


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
    editor::Init();


    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        input::mouse_left_down = false;
        input::mouse_left_up = false;
        input::mouse_right_down = false;
        input::mouse_right_up = false;

        for (auto& key : input::current_keys) {
            if (key.second == 1)  key.second = 2;
            if (key.second == -1) key.second = 0;
        }

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
                input::mouse_x = e.motion.x;
                input::mouse_y = e.motion.y;
                input::mouse_block_x = input::mouse_x / 16;
                input::mouse_block_y = input::mouse_y / 16;
                input::mouse_grid_x = input::mouse_block_x * 16;
                input::mouse_grid_y = input::mouse_block_y * 16;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    input::mouse_left_down = true;
                    input::mouse_left_pressed = true;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    input::mouse_right_down = true;
                    input::mouse_right_pressed = true;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    input::mouse_left_up = true;
                    input::mouse_left_pressed = false;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT)
                {
                    input::mouse_right_up = true;
                    input::mouse_right_pressed = false;
                }
            }
            //User presses a key
            else if (e.type == SDL_KEYDOWN)
            {
                //Select surfaces based on key press
                input::current_keys[SDL_GetKeyName(e.key.keysym.sym)] = 1;
            }
            else if (e.type == SDL_KEYUP)
            {
                //Select surfaces based on key press
                input::current_keys[SDL_GetKeyName(e.key.keysym.sym)] = -1;
            }
        }
        if (quit) break;

        // Logic state machine!
        switch (state) {
            case MODE_MAPEDITOR:
                state = editor::Logic();
                break;
            case MODE_TILESETEDITOR:
                state = tileset_editor::Logic();
                break;
        };

        //Clear screen
        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
        SDL_RenderClear(g_renderer);

        // Rendering state machine!
        switch (state) {
            case MODE_MAPEDITOR:
                editor::Render();
                break;
            case MODE_TILESETEDITOR:
                tileset_editor::Render();
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