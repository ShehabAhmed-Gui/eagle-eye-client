#ifndef GAME_H
#define GAME_H

#include "cheat.h"
#include "game_map.h"
#include "player.h"

#include <raylib.h>

#include <string>


class Game
{
private:
    EagleEye::Connection ac_connection;

    void init_anticheat();
public:
    GameMap map;

    Texture2D background;
    Player player;

    Game();
    ~Game();

    void loop();

    void update();
    void draw();
    void draw_map();
};

#endif