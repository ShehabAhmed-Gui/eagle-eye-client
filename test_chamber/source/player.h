#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

struct Player
{
    float x;
    float y;
    int w;
    int h;

    Vector2 velocity;
    Vector2 origin; 

    float speed;
    float acceleration;
    float friction; //how fast the player stops horizontal movement
    float gravity;
    float jump_force;
    float rotation;
    float rotation_speed;
    float max_rotation;

    Color color;

    Player();
    void update();
    void draw();
    void collide();
    bool on_ground();

    void accelerate(int dir);
    void halt_x_movement();
};

#endif