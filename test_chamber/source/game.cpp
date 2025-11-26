#include <raylib.h>
#include <cstdlib>
#include <memory.h>

#include <algorithm>
#include <iostream>
#include <fstream>

#include "game.h"
#include "gameconst.h"
#include "player.h"

void game_loop()
{
    Player player = Player();
    GameMap map = GameMap(WINDOW_WIDTH / TILE_SIZE.x, WINDOW_HEIGHT / TILE_SIZE.y);
    map.load_from_file("example_map.txt");

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        player.update(map);
        player.draw();

        draw_map(map);

        EndDrawing();
    }
}

void draw_map(GameMap& map)
{
    for (int y = 0; y < map.h; y++)
    {
        for (int x = 0; x < map.w; x++)
        {
            int id = map.get_tile(x, y);
            if (id == -1) {
                continue;
            }

            Color color = WHITE;

            DrawRectangle(x * TILE_SIZE.x, y * TILE_SIZE.y,
                          TILE_SIZE.x, TILE_SIZE.y,
                          color);
        }
    }
}

GameMap::GameMap(int w, int h)
{
    this->w = w;
    this->h = h;
    tiles = new int[w * h];

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            tiles[i + (j * w)] = -1;
        }
    }
}

GameMap::~GameMap()
{
    delete tiles;
}

int GameMap::get_tile(int x, int y) {
    int index = x + (y * w);
    if (index > (w * h)) {
        return -1;
    }

    return tiles[index];
}

bool GameMap::set_tile(int x, int y, int id) {
    int index = x + (y * w);
    if (index > (w * h)) {
        return false;
    }
    
    tiles[index] = id;
    return true;
}

void GameMap::load_from_file(const std::string path)
{
    std::ifstream file(path);
    if (!file) {
        std::cout << "WARNING: Opening map file " << path << " failed." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        if (line.rfind('#', 0) == 0) {
            continue;
        }
        if (line.empty()) {
            continue;
        }
        
        std::cout << line << std::endl;
        std::size_t equal_pos = line.find('=');
        if (equal_pos == std::string::npos) {
            std::cout << "WARNING: Invalid line in map data: '" << line << "'" << std::endl;
            continue;
        }

        std::string coords = line.substr(0, equal_pos);
        std::string tile = line.substr(equal_pos + 1);

        if (tile.empty() || coords.empty()) {
            std::cout << "WARNING: Invalid line in map data: '" << line << "'" << std::endl;
            continue;
        }

        std::size_t comma_pos = coords.find(',');
        if (comma_pos == std::string::npos) {
            std::cout << "WARNING: Invalid line in map data: '" << line << "'" << std::endl;
            continue;
        }

        std::string x_str = coords.substr(0, comma_pos);
        std::string y_str = coords.substr(comma_pos + 1);
//        std::cout << "X: " << x_str << " Y: " << y_str << " ID: " << tile << std::endl;

        int x = std::stoi(x_str);
        int y = std::stoi(y_str);
        int id = std::stoi(tile);

        set_tile(x, y, id);
    }
}
