#ifndef BULLET_H
#define BULLET_H

class Bullet: public GameObject
{
    // This class just services as an abstraction for the bullet sprites
    // it inherits the GameObject class
    public:

        Bullet(SDL_Window * window):GameObject(window){}
        void selfDestruct()
        {
            delete this;
        }

    private:
};



#endif // BULLET_H
