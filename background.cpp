
#ifndef BACKGROUND_H
#define BACKGROUND_H

// This class handles loading and background movement

class Background
{
    public:
        Background(SDL_Window * window,const char * filename);
        void drawFrame();
        void selfDestruct()
        {
            delete this;
        }

    private:
        SDL_Rect dest,src;
        SDL_Renderer * bRenderer;
        SDL_Texture * bTexture;
        int width,height;
        ~Background()
        {
            this->selfDestruct();
        }
};

Background::Background(SDL_Window * window,const char * filename)
{
    bRenderer = SDL_GetRenderer(window);// Copy the main renderer
    SDL_Surface * image = IMG_Load(filename);// Load background image

    if(image == NULL)
    {
        fprintf(stderr,"Error, Unable to load background image: %s\n",SDL_GetError());
        exit(1);
    }
    // create background texture
    bTexture = SDL_CreateTextureFromSurface(bRenderer,image);
    if(!bTexture)
    {
        fprintf(stderr,"Error, Unable to create Background texture: %s\n",SDL_GetError());
        exit(1);
    }

    SDL_QueryTexture(bTexture,NULL,NULL,&src.w,&src.h);
    // Save original width and height
    width = src.w;
    height = src.h;

    // divide background image into 2 halfs;
    src.h /= 2;
    src.x = 0;
    src.y = src.h/2;
    // copy window dimensions
    SDL_GetWindowSize(window,&dest.w,&dest.h);
    dest.x = dest.y = 0;
}


void Background::drawFrame()
{
    // check if the y position of the src rect <= 0.
    // if so decrement the y position of the src rect
    // if the src rect reaches the top of the original image( at y = 0)
    // set the src rect y position to half the height of the original image

//        if(src.y <= 0)
//        {
//            src.y = height/2;
//        }
//        else
//        {
//            --src.y;
//        }

// for more info checkout this site
// 'https://lazyfoo.net/tutorials/SDL/31_scrolling_backgrounds/index.php'

    src.y = (src.y <= 0)?src.y=height/2:src.y-=3;

    SDL_RenderCopy(bRenderer,bTexture,&src,&dest);
//    printf("Index: %d\n",src.y);
}

#endif //BACKGROUND_H
