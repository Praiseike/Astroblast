#include "GameObject.h"
#include "bullet.cpp"
#ifndef SHIP_H
#define SHIP_H

class Ship:public GameObject
{
    public:
        Ship(SDL_Window * window);
        Bullet * bullet;
        void moveLeft();    // overwrite GameObject moveLeft
        void moveRight();   // overwrite GameObject moveRight
        void draw();
        int health = 100;   // health
        ~Ship()
        {
            // Destroy all textures
            SDL_DestroyTexture(textures[0]);
            SDL_DestroyTexture(textures[1]);
            SDL_DestroyTexture(textures[2]);
        }

    private:
        bool lean_right = false;
        bool lean_left = false;


        enum { left,center,right};
        SDL_Texture *textures[3];
};

// I had to overide the move methods in GameObject
// So as to let me switch textures for the ship

void Ship::moveRight()
{
    // set lean_right to true and move the ship right
    lean_right = true;
    if(dest.x < this->window.w - dest.w)
        dest.x += this->speed;
}

void Ship::moveLeft()
{
    // similar thing here
    lean_left = true;
    if(dest.x >= 0)
        dest.x -= this->speed;
}

void Ship::draw()
{
    // originally set the current texture to center
    this->gTexture = textures[center];

    //check if lean_left of lean_right is true
    if(lean_left == true)
    {
        this->gTexture = textures[left];
    }
    if(lean_right == true)
    {
        this->gTexture = textures[right];
    }
    // draw the chosen texture
    SDL_RenderCopy(this->gRenderer,this->gTexture,&this->src,&this->dest);
    // reset the decision
    lean_left = false;
    lean_right = false;
}

Ship::Ship(SDL_Window * window):GameObject(window)
{
    textures[left] = loadTexture("assets/hero3_left.png");
    textures[right] = loadTexture("assets/hero3_right.png");
    textures[center] = loadTexture("assets/hero3.png");
    dest.w = src.w /8;  // the original ship size is huge that's why;
    dest.h = src.h/8;   // i reduced to 1/8 its size;
}

#endif // SHIP_H
