// This should contain everything to do with graphics
#include "graphics.h"
#include <stdio.h> // Standard IO
#include <SDL.h> // SDL
#include <SDL_image.h> // SDL_Image
#include <SDL_ttf.h> // SDL_ttf
#include <string>

namespace graphics {

    TTF_Font* font;

    void Init() {
        font = TTF_OpenFont("assets/pixelmix.ttf", 8);
    }

    SDL_Texture* LoadImage(const char* path)
    {
        SDL_Texture* new_texture = NULL;
        std::string modified_path = "assets/";
        modified_path += path;
        SDL_Surface* loaded_image_surface = IMG_Load(modified_path.c_str());
        if (loaded_image_surface == NULL)
        {
            printf("Unable to load image %s! SDL Error: %s\n", modified_path.c_str(), SDL_GetError());
            return nullptr;
        }
        new_texture = SDL_CreateTextureFromSurface(g_renderer, loaded_image_surface);
        if (new_texture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", modified_path.c_str(), SDL_GetError());
            return nullptr;
        }
        //Get rid of old loaded surface
        SDL_FreeSurface(loaded_image_surface);
        return new_texture;
    }

    void DrawImage(SDL_Texture* image, int x, int y)
    {
        int w, h;
        SDL_QueryTexture(image, NULL, NULL, &w, &h);
        SDL_Rect dstrect{ x, y, w, h };
        SDL_RenderCopy(g_renderer, image, NULL, &dstrect);
    }

    void DrawImagePart(SDL_Texture* image, int x, int y, int x2, int y2, int w, int h)
    {
        SDL_Rect srcrect{ x2, y2, w, h };
        SDL_Rect dstrect{ x, y, w, h };
        SDL_RenderCopy(g_renderer, image, &srcrect, &dstrect);
    }

    bool DrawText(TTF_Font* font, int x, int y, std::string text, int r, int g, int b, int a) {
        SDL_Color color = { (Uint8) r, (Uint8) g, (Uint8) b };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

        SDL_Surface* surface_text = TTF_RenderText_Solid(font, text.c_str(), color); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

        SDL_Texture* texture_text = SDL_CreateTextureFromSurface(g_renderer, surface_text); //now you can convert it into a texture

        int w, h;
        TTF_SizeText(font, text.c_str(), &w, &h);

        SDL_Rect text_rect; //create a rect
        text_rect.x = x;  //controls the rect's x coordinate 
        text_rect.y = y; // controls the rect's y coordinte
        text_rect.w = w; // controls the width of the rect
        text_rect.h = h; // controls the height of the rect

        //Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understand

        //Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

        SDL_RenderCopy(g_renderer, texture_text, NULL, &text_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

        //Don't forget to free your surface and texture
        SDL_FreeSurface(surface_text);
        SDL_DestroyTexture(texture_text);
        return true;
    }

    bool DrawText(TTF_Font* font, int x, int y, std::string text, int r, int g, int b) {
        return graphics::DrawText(font, x, y, text, r, g, b, 255);
    }

    void DrawTextBordered(TTF_Font* font, int x, int y, std::string text, int r, int g, int b, int a) {
        DrawText(font, x - 1, y, text, 0, 0, 0, a);
        DrawText(font, x + 1, y, text, 0, 0, 0, a);
        DrawText(font, x, y - 1, text, 0, 0, 0, a);
        DrawText(font, x, y + 1, text, 0, 0, 0, a);
        DrawText(font, x, y, text, r, g, b, a);
    }

    void DrawTextBordered(TTF_Font* font, int x, int y, std::string text, int r, int g, int b) {
        DrawTextBordered(font, x, y, text, r, g, b, 255);
    }

    void DrawRectangle(int x, int y, int w, int h, int r, int g, int b, int a) {
        SDL_SetRenderDrawColor(g_renderer, r, g, b, a);
        SDL_Rect rectangle;
        rectangle.x = x;
        rectangle.y = y;
        rectangle.w = w;
        rectangle.h = h;
        SDL_RenderDrawRect(g_renderer, &rectangle);
    }
    void DrawRectangle(int x, int y, int w, int h, int r, int g, int b) {
        DrawRectangle(x, y, w, h, r, g, b, 255);
    }
}