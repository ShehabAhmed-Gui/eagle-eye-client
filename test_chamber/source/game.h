#ifndef GAME_H
#define GAME_H

#include <string>

struct GameMap
{
    int* tiles;
    int w;
    int h;

    GameMap(int w, int h);
    ~GameMap();
    int GameMap::get_tile(int x, int y);
    bool GameMap::set_tile(int x, int y, int id);

    void load_from_file(const std::string path);
};

void game_loop();
void draw_map(GameMap& map);

#endif