#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H
#include <iostream>
#include <string>

using namespace std;

class FontManager
{
    public:
        FontManager(SDL_Window * window,const char * filename,int fontSize);
        void drawVariable(std::string str,int value,int x,int y,int w,int h);
        void writeString(std::string str,int x,int y,int w,int h);
        ~FontManager()
        {
            SDL_DestroyTexture(texture);
        }
    private:
        SDL_Rect rect;  // font rect
        TTF_Font * font;  // TTF_Font object
        SDL_Texture * texture;  // font texture
        SDL_Color color;    // font color
        SDL_Renderer * renderer;    // renderer
};


// The constructor does most of the initialization
FontManager::FontManager(SDL_Window * window,const char * filename,int fontSize)
{
    TTF_Init(); // Initialize the SDL2_ttf library

    // Loads a font file and returns the pointer to a TTF_Font object
    font = TTF_OpenFont(filename,fontSize);
    color = {0xff,0xff,0xff};// Set the color
    renderer = SDL_GetRenderer(window);// Get the current window renderer
}

// Draws a variable to the screen in text
void FontManager::drawVariable(std::string str,int value,int x,int y,int w,int h)
{
    // copy and convert int value to string
    std::string s= str;
    s += std::to_string(value);

    // generate a c string pointer
    const char * data = s.c_str();
    // Set text dimensions
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    // create a surface with the rendered text
    SDL_Surface * temp = TTF_RenderText_Solid(font,data,color);

    // Since we are using hardware renderering, all surfaces should be
    // converted to textures
    texture = SDL_CreateTextureFromSurface(renderer,temp);
    SDL_FreeSurface(temp);// free temp textures
    SDL_RenderCopy(renderer,texture,NULL,&rect);    // render text to the main window
    SDL_DestroyTexture(texture);    // destroy the texture to allow updates
}

void FontManager::writeString(std::string str,int x,int y,int w,int h)
{
    // the same as drawVariable except that we dont renderer variables,
    // just text.
    const char * data = str.c_str();
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_Surface * temp = TTF_RenderText_Solid(font,data,color);
    texture = SDL_CreateTextureFromSurface(renderer,temp);
    SDL_FreeSurface(temp);
    SDL_RenderCopy(renderer,texture,NULL,&rect);
    SDL_DestroyTexture(texture);

}

#endif // FONT_MANAGER_H
