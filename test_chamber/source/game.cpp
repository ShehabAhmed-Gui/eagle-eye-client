/* Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <raylib.h>
#include <cstdlib>
#include <memory.h>

#include <iostream>

#include "cheat.h"
#include "game.h"
#include "gameconst.h"
#include "player.h"

Game::Game()
{
    init_anticheat();

    map.init(WINDOW_WIDTH / TILE_SIZE.x, WINDOW_HEIGHT / TILE_SIZE.y);
    map.print();
    map.load_from_file("example_map.txt");

    background = LoadTexture("background.png");
}

Game::~Game()
{
    map.clear();
    UnloadTexture(background);
}

void Game::loop()
{
    while (WindowShouldClose() == false)
    {
        update();
        draw();
    }
}

void Game::update()
{
    player.update(map);
}

void Game::draw()
{
    Rectangle background_source = {0, 0, background.width, background.height};
    Rectangle background_dest = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);
    DrawTexturePro(background, background_source, background_dest, {0, 0}, 0, WHITE);

    draw_map();

    player.draw();

    EndDrawing();
}

void Game::init_anticheat()
{
    ac_connection = EagleEye::Connection();

    if (EagleEye::is_anticheat_running()) {
        std::cout << "INFO: Anticheat service is running.\n";

        ac_connection.connect();
        if (ac_connection.send_token_request() == true) {
            std::cout << "INFO: Sent token request to anticheat service.\n";
        }
    }
    else {
        std::cout << "INFO: Anticheat service is NOT running.\n";
    }
}

void Game::draw_map()
{
    int map_w = map.width();
    int map_h = map.height();
    for (int y = 0; y < map_h; y++)
    {
        for (int x = 0; x < map_w; x++)
        {
            int id = map.get_tile(x, y);
            if (id == -1) {
                continue;
            }

            Color color = BROWN;

            DrawRectangle(x * TILE_SIZE.x, y * TILE_SIZE.y,
                          TILE_SIZE.x, TILE_SIZE.y,
                          color);
        }
    }
}