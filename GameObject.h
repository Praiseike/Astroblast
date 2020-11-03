
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#ifndef GAME_OBJECT_H

#define GAME_OBJECT_H
#define GAME_OBJECT_ANIMATION 0x01
#define GAME_OBJECT_STATIC 0x02
#define GAME_OBJECT_ANIMATION_STATIC 0x03

class GameObject
{
    public:
        GameObject(SDL_Window * window);
        SDL_Texture * loadTexture(const char * filename);
        SDL_Rect src,dest;
        void draw();

        // move object left
        inline void moveLeft() {
            if(this->dest.x > 0)
            this->dest.x -= this->speed;
        }
        // move object right
        inline void moveRight() {
            if(this->dest.x < window.w - dest.w)
                this->dest.x += this->speed;
        }

        // move object up
        inline void moveUp() {
            if(this->dest.y > 0)
                this->dest.y -= this->speed;
        }

        // move object down
        inline void moveDown() {
            if(this->dest.y < window.h - dest.h)
                this->dest.y += this->speed;
        }


        // set object position
        inline void setPosition(int x,int y)
        {
            this->dest.x = x;
            this->dest.y = y;
        }

        // set object movement speed
        inline void setSpeed(int s)
        {
            if(s < this->speed)
                this->speed = 5;
            else
                this->speed = s;
        }

        void setTexture(SDL_Texture * texture);

    protected:
        SDL_Renderer * gRenderer = NULL;
        SDL_Texture * gTexture = NULL;
        SDL_Rect window;
        ~GameObject();
        int speed = 5;

};

GameObject::GameObject(SDL_Window * Window)
{
    // copy the window dimensions and renderer
    SDL_GetWindowSize(Window,&this->window.w,&this->window.h);
    gRenderer = SDL_GetRenderer(Window);
    src.x = src.y = 0;
}

SDL_Texture * GameObject::loadTexture(const char * filename)
{
    // load a texture to memory
    SDL_Surface * image = IMG_Load(filename);
    if(!image) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Unable to load image: %s",SDL_GetError());
    this->gTexture = SDL_CreateTextureFromSurface(this->gRenderer,image);
    if(this->gTexture == NULL)
    {
        printf("Unable to create Texture: %s\n",SDL_GetError());
        exit(0);
    }

    SDL_FreeSurface(image);
    // Get texture dimensions
    SDL_QueryTexture(this->gTexture,NULL,NULL,&this->src.w,&this->src.h);
    dest.w = src.w;
    dest.h = src.h;
    return this->gTexture;
}

void GameObject::draw()
{
        SDL_RenderCopy(this->gRenderer,this->gTexture,&this->src,&this->dest);
}

void GameObject::setTexture(SDL_Texture * texture)
{
    // get texure dimensions
    SDL_QueryTexture(texture,NULL,NULL,&this->src.w,&this->src.h);
    dest.w = src.w;
    dest.h = src.h;
    src.x = src.y = dest.x = dest.y = 0;
    // create an empty texture
    this->gTexture = SDL_CreateTexture(this->gRenderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,src.w,src.h);
    // set the rendering target to the previously created texture
    SDL_SetRenderTarget(this->gRenderer,this->gTexture);
    // copy the pixel data from the loaded texture to the
    // preivously created texture
    SDL_RenderCopy(this->gRenderer,texture,&src,&dest);
    // Iniitiate the copy action :)
    SDL_RenderPresent(gRenderer);
    // reset render target to the main window
    SDL_SetRenderTarget(gRenderer,NULL);
}


GameObject::~GameObject()
{
    //SDL_DestroyTexture(this->gTexture);
}

#endif // GAME_OBJECT_H
