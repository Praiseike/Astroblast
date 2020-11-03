/// Author: Praise Ike
/// Email: Praiseike123@gmail.com


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "Animate.cpp"
#include "ship.cpp"
#include "background.cpp"
#include "audioManager.cpp"
#include "fontManager.cpp"

#define WIDTH 1000
#define HEIGHT 760

#define MAX_BULLETS 100
#define MAX_EXPLOSIONS 12
#define MAX_ASTROIDS 12

using namespace std;

SDL_Renderer *gRenderer = NULL;
SDL_Window * gWindow = NULL;

FontManager * font;

// Textures
SDL_Texture * explosionTexture = NULL;
SDL_Texture * astroidTexture = NULL;
SDL_Texture * bulletTexture = NULL;

// Managers and other classes
AudioManager * audioStore = NULL;
Ship * hero;
Background * background;
Animation * animator;
Animation * astroidFleet[12];


// Don't waste the stack space;
//Bullet ** bulletSprites = (Bullet**)malloc(sizeof(Bullet*)*MAX_BULLETS);
// Try new ways
//Bullet ** bulletSprites = new Bullet*[sizeof(Bullet*)*MAX_BULLETS];
Bullet * bulletSprites[2][MAX_BULLETS];

Animation * explosions[MAX_EXPLOSIONS];

// game state variables
bool running = true;
bool paused = false;
int score = 0;
bool gameOver = false;
float difficulty_level = 1;

using namespace std;

// complain
void error()
{
    fprintf(stderr,"ERROR: %s\n",SDL_GetError());
    exit(1);
}

void render()
{

        SDL_RenderClear(gRenderer);

        background->drawFrame();
        // render astroids
        for(int i = 0;i<MAX_ASTROIDS;i++)
        {
            if(astroidFleet[i] != NULL)
                astroidFleet[i]->drawFrame();
        }

        //render bullets
        for(int j = 0;j< 2;j++)
        {
            for(int i = 0;i< MAX_BULLETS;i++)
            {
                if(bulletSprites[j][i] != NULL)
                {
                    bulletSprites[j][i]->draw();
                }
            }
        }
        // render explosions
        for(int i = 0;i<MAX_EXPLOSIONS;i++)
        {
            if(explosions[i] != NULL)
                explosions[i]->drawFrame();
        }
        if(hero != NULL)
            hero->draw();
        font->drawVariable("Score: ",score,0,0,200,40);
        font->drawVariable("Health: ",hero->health,WIDTH-200,0,200,40);
        if(gameOver)
            font->writeString("GameOver",(WIDTH/2)-300,HEIGHT/2,600,70);
        SDL_RenderPresent(gRenderer);
}


void fire(Ship * hero)
{
        // bullet->loadTexture("assets/bullet.bmp");
        // I didn't like the idea of having to load textures from disk
        // each time it fired,
        // that's why I created textures for most objects before hand.
        // They just need to copy those textures already in memory
    Bullet *bullet[2];

    for(int i = 0;i < 2;i++)
    {
        bullet[i] = new Bullet(gWindow);
        bullet[i]->setTexture(bulletTexture);// Copy the global bulletTexture;
        bullet[i]->setSpeed(14);
    }

    // set bullet positions
    bullet[0]->setPosition(hero->dest.x+10,hero->dest.y+40);
    bullet[1]->setPosition(hero->dest.x+60,hero->dest.y+40);

    // Scan for free space and give bullet instance
    for(int j = 0;j < 2;j++)
    {
        for(int i = 0;i < MAX_BULLETS;i++)
        {
            if(bulletSprites[j][i] == NULL)
            {
                bulletSprites[j][i] = bullet[j];
                break;
            }
        }
    }
}

void explode(Animation * astroid,int index)
{
    animator = new Animation(gWindow,explosionTexture,16,125,ONE_CYCLE);
    animator->dest.x = astroid->dest.x;
    animator->dest.y = astroid->dest.y;

    // If an explosion animation instance already occupied space for the new animation
    // Delete the old animation instance and set create a new one for the new explosion

    // Each cell in the explosion array is mapped to the astroidSprites array
    // That's why I included the index for the dying astroid :(
    // But it turns out my implementation of the Animation class is not efficient
    // Cuz i've noticed lags in performance during the explosion animation interval.

    // Please if you have a solution to this, contact me through the email at
    // The top of this source

    if(explosions[index] != NULL)
    {
        delete explosions[index];
        explosions[index] = NULL;   // Just in case
    }
    explosions[index] = animator;
}

void reset()
{
    // soft reset
    gameOver = false;
    paused = false;
    score = 0;
    hero->health = 100;
    difficulty_level = 1;
    hero->setPosition(WIDTH/2-(hero->dest.w/2),HEIGHT-(hero->dest.h));
}

void update()
{

    // move the valid bullets upwards;
    for(int j = 0;j< 2;j++)
    {
        for(int i = 0;i<MAX_BULLETS;i++)
        {
            if(bulletSprites[j][i] != NULL)
            {
                bulletSprites[j][i]->moveUp();
            }
        }
    }

    // move the astroids downwards
    for(int i = 0;i<MAX_ASTROIDS;i++)
    {
        if(astroidFleet[i] != NULL)
            astroidFleet[i]->dest.y += difficulty_level;
    }


    // If an astroid gets past the hero
    // decrement hero's health
    // play sound effect for event
    // destroy the astroid
    // set the pointer to null
    for(int i = 0;i <MAX_ASTROIDS;i++)
    {
        if(astroidFleet[i] != NULL && astroidFleet[i]->dest.y >= HEIGHT)
        {
            if(hero->health >= 0)
                hero->health -= 10;
            audioStore->playSound("been_hit");
            astroidFleet[i]->selfDestruct();
            astroidFleet[i] = NULL;
        }
    }
}

void createFleet(Animation ** astroidFleet)
{
    // create new animation objects in the array
    for(int i = 0;i < MAX_ASTROIDS;i++)
    {
        astroidFleet[i] = new Animation(gWindow,astroidTexture,16,60,INFINITE);
        astroidFleet[i]->dest.x = rand() % (WIDTH-100);// give random positions
        astroidFleet[i]->dest.y = -1*(rand()%190)-200;// so they don't lineup straight
    }
}

void logic()
{

    // Check collisions
    // loop 1: for both bullet arrays
    for(int j = 0;j< 2;j++)
    {
        // loop 2: bullets for each array
        for(int i = 0;i<MAX_BULLETS;i++)
        {
            if(bulletSprites[j][i] != NULL)
            {
                // loop 3: for each astroidSprites with current bullet
                for(int k = 0;k<MAX_ASTROIDS;k++)
                {
                    // If collision detected
                    if(SDL_HasIntersection(&astroidFleet[k]->dest,&bulletSprites[j][i]->dest))
                    {
                        audioStore->playSound("explosion");// play explosion effect
                        explode(astroidFleet[k],k); // animate explosion
                        astroidFleet[k]->selfDestruct();// commit suicide
                        astroidFleet[k] = NULL;// reset address to avoid confusion
                        score+= 25; // increment the score
                        bulletSprites[j][i]->selfDestruct();    // commit suicide
                        bulletSprites[j][i] = nullptr;
                        break;
                    }
                }
            }
        }
    }
    // Clean the expired bullets
    for(int j = 0;j< 2;j++)
    {
        for(int i = 0;i< MAX_BULLETS;i++)
        {   // if sprite object exist
            if(bulletSprites[j][i] != NULL)
            {
                // if it pass the top window border
                if(bulletSprites[j][i]->dest.y <= 0)
                {
                    // destroy the bullet object
                    bulletSprites[j][i]->selfDestruct();
                    bulletSprites[j][i] = NULL;
                }
            }
        }
    }

    // Recreate fleet
    int isEmpty = true;
    for(int i = 0;i < MAX_ASTROIDS;i++)
    {
        // Scan all through to check if empty
        if(astroidFleet[i] != NULL)
            isEmpty = false;
    }

    // check if all astroids were destroyed
    if(isEmpty == true && !gameOver)
    {
        difficulty_level += 0.2;
        createFleet(astroidFleet);
    }

    // check if play failed
    if(hero->health <= 0)
    {
        gameOver = true;
        paused = true;
        // destroy all astroids
        for(int i = 0;i < MAX_ASTROIDS;i++)
        {
            if(astroidFleet[i] != NULL)
            {
                astroidFleet[i]->selfDestruct();
                astroidFleet[i] = NULL;
            }
        }
    }
}

void die();

void handleEvents(SDL_Event event)
{
    if(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            running = false;
            die();
        }
        if(event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym == SDLK_SPACE && gameOver)
            {
                reset();
            }
            if(event.key.keysym.sym == SDLK_p)
                paused = !paused;
            if(event.key.keysym.sym == SDLK_s)
            {
                // Get a screen shot
                SDL_Surface * screen = SDL_CreateRGBSurface(0,WIDTH,HEIGHT,32,0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
                SDL_RenderReadPixels(gRenderer,NULL,SDL_PIXELFORMAT_ARGB8888,screen->pixels,screen->pitch);
                string s = to_string(time(NULL));
                string str = "screenshots/screen_shot_"+s+".png";
                IMG_SavePNG(screen,str.c_str());
                SDL_FreeSurface(screen);
            }

        }
    }
    static int fireCount = 0;// for bullet pacing

    if(paused == false)
    {
        const Uint8 * keystate = SDL_GetKeyboardState(NULL);

        if(keystate[SDL_SCANCODE_LEFT])
            hero->moveLeft();
        if(keystate[SDL_SCANCODE_RIGHT])
            hero->moveRight();
        if(keystate[SDL_SCANCODE_DOWN])
            hero->moveDown();
        if(keystate[SDL_SCANCODE_UP])
            hero->moveUp();

        if(keystate[SDL_SCANCODE_SPACE])
        {
            if(fireCount%4 == 0)    // fire when fireCount = 4 and reset fireCount to 0;
            {
                fire(hero);
                fireCount = 0;
            }
        }
        fireCount++;
    }
}


void die()
{
    for(int i = 0;i < MAX_EXPLOSIONS;i++)
    {
        if(explosions[i] != NULL)
            delete explosions[i];
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(astroidTexture);
    SDL_DestroyTexture(explosionTexture);
    SDL_Quit();
    cout << "Die function called"<<endl;
    exit(EXIT_SUCCESS); // seems like forcing an exit

}

int main()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        error();
    printf("Created window Successfully\n");
    // Create window
    SDL_Window * window = SDL_CreateWindow("My window",0,0,WIDTH,HEIGHT,SDL_WINDOW_SHOWN |SDL_WINDOW_FULLSCREEN);
    if(!window)
        error();

    gWindow = window;// Copy the window to the global scope
    SDL_Renderer * renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
    if(!renderer)
        error();

    SDL_Event event;

    gRenderer = renderer;// Also copy the renderer to the global scope

    // Audio player setup

    audioStore = new AudioManager();// Initialize my audio masterpiece :)
    audioStore->registerMusic("music/gameplay.wav","bg_music");// Register music
    audioStore->registerSound("music/explosion.wav","explosion");// Register sound effect
    audioStore->registerSound("music/exploPop.wav","been_hit",90);
    audioStore->playMusic("bg_music"); // play the background music
    // See audioManager.cpp for more implementation detais


    // Create the ship object
    // It inherits the GameObject class
    hero = new Ship(window);
    hero->setPosition((WIDTH/2)-(hero->dest.w/2),HEIGHT-(hero->dest.h));
    hero->setSpeed(10);

    //load the background image
    background = new Background(window,"assets/parallax_bg.png");

    // seed the random number generator;
    srand(time(NULL));

    // Clearing bullets and explosions array
    for(int j = 0;j< 2;j++)
    {
        for(int i = 0;i< MAX_BULLETS;i++)
        {
            bulletSprites[j][i] = NULL;
            explosions[i] = NULL;
        }
    }

    SDL_Surface * temp = IMG_Load("assets/astroid.png");

    if(!temp)
        error();

    // I don't want to have to load textures every time for every instance
    astroidTexture = SDL_CreateTextureFromSurface(renderer,temp);
    SDL_FreeSurface(temp);
    temp = IMG_Load("assets/explosion.png");
    explosionTexture = SDL_CreateTextureFromSurface(renderer,temp);
    SDL_FreeSurface(temp);
    temp = IMG_Load("assets/bullet2.png");
    bulletTexture = SDL_CreateTextureFromSurface(renderer,temp);
    SDL_FreeSurface(temp);
    if(!astroidTexture || !explosionTexture || !bulletTexture)
    {
        printf("Unable to create textures: %s\n",SDL_GetError());
    }

    //Create the fleet
    createFleet(astroidFleet);

    printf("Entering gameloop\n");


    // Font
    font = new FontManager(gWindow,"fonts/NotoSans-Regular.ttf",200);

    while(running)
    {
        if(paused == false)
        {
            logic();
            update();
            render();
        }
        if(paused == true)
            SDL_Delay(100); // ease load on processor
        handleEvents(event);
    }
    return 0;
}
