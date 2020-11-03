#ifndef ANIMATE_H
#define ANIMATE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#define INFINITE 0x01
#define ONE_CYCLE 0x02

// This class Handles simple animtions for the astroid sprites
// It also handles the explosions

// they are two types of animations
// * the Infinely linear animations (astroid sprites)
// * the other displays for only a cycle    (explosions)

class Animation
{
    public:
        Animation(SDL_Window * window,SDL_Texture * anim_texture,int frameCount,int frameWidth,int flag);
        void drawFrame();
        void selfDestruct() { delete this; }
        SDL_Rect dest,src;

    private:
        Uint8 flag;
        SDL_Window * aWindow = NULL;    // Local window pointer
        SDL_Renderer * aRenderer = NULL;    // Local renderer pointer
        SDL_Texture * texture = NULL;   // Local texture
        short int anim_index = 0;   //  what frame we are in
        int frame_count;        // No of frames in animtion
        int frame_width;        // width of each frame
};

Animation::Animation(SDL_Window * window,SDL_Texture * anim_texture,int frameCount,int frameWidth,int flag = INFINITE)
{
    // Copy the main window and renderer
    aWindow = window;
    aRenderer = SDL_GetRenderer(aWindow);

    // set inner variables
    this->flag = flag;
    this->frame_count = frameCount;
    this->frame_width = frameWidth;

//    SDL_Surface * image = IMG_Load(filename);
//    if(!image)
//        fprintf(stderr,"Error: %,s\n",SDL_GetError());
//    texture = SDL_CreateTextureFromSurface(aRenderer,image);
//    if(!texture)

    // Copy previously loaded texture
    texture = anim_texture;
    SDL_QueryTexture(texture,NULL,NULL,&src.w,&src.h);
    src.x = src.y = 0;
    // set frame width and height
    src.w = src.h = frameWidth;
    dest.h = src.h;
    dest.w = src.w;

}


void Animation::drawFrame()
{
    // If the INIFINITE flag is set
    if(flag == INFINITE)
    {
        // select current frame
        int frame = int((SDL_GetTicks()/100)%frame_count);
        src.x = src.w * frame;// move frame by frame_width
        SDL_RenderCopy(aRenderer,texture,&src,&dest);// render current frame
    }
    // If ONE_CYCLE is set
    if(flag == ONE_CYCLE)
    {
        // anim_index acts as a counter
        if(this->anim_index < frame_count)
        {
            // select frame
            int frame = int((SDL_GetTicks()/100)%frame_count);
            src.x = src.w * frame;// move frame by frame_width
            SDL_RenderCopy(aRenderer,texture,&src,&dest);
            SDL_Delay(10);// delay so it wont be supper fast
            this->anim_index +=2; // increment counter
        }
    }
}

#endif // ANIMATE_H
