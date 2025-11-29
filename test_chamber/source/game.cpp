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
#include "vendor/json.hpp"

Game::Game()
{
    state = GameState::PLAY;
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
        if (ac_connection.is_empty()) {
            ac_connection.connect();
        }

        std::string msg = ac_connection.read_message();
        if (msg.empty() == false) {
            handle_anticheat_message(msg);
        }

        update();
        draw();
    }
}

void Game::update()
{
    switch (state)
    {
        case PLAY:
        {
            player.update(map);
        } break;

        case BANNED:
        {

        } break;
    }
}

void Game::draw()
{
    BeginDrawing();

    switch (state)
    {
        case PLAY:
        {
            Rectangle background_source = {0, 0, static_cast<float>(background.width), static_cast<float>(background.height)};
            Rectangle background_dest = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

            ClearBackground(BACKGROUND_COLOR);
            DrawTexturePro(background, background_source, background_dest, {0, 0}, 0, WHITE);

            draw_map();

            player.draw();
        } break;

        case BANNED:
        {
            ClearBackground(BLACK);

            Color color = {200, 20, 20, 255};
            draw_centered_text("Violation Detected", 0.25f, 48, color);

            draw_centered_text(m_details.c_str(), 0.5f, 24, color);
        } break;
    }


    EndDrawing();
}

void Game::draw_centered_text(const char* text, float y_factor, int font_size, Color color)
{
    int text_width = MeasureText(text, font_size);
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    // Calculate centered position
    int x = (screen_width - text_width) / 2;
    int y = (screen_height - font_size) * y_factor;

    // Draw centered text
    DrawText(text, x, y, font_size, color); 
}

void Game::init_anticheat()
{
    ac_connection = EagleEye::Connection();

    if (!EagleEye::is_anticheat_running()) {
        std::cout << "INFO: Anticheat service is NOT running.\n";
        on_violation("Anti-cheat service is not running");
        return;
    }

    std::cout << "INFO: Anticheat service is running.\n";

    ac_connection.connect();
    if (ac_connection.send_token_request() == true) {
        std::cout << "INFO: Sent token request to anticheat service.\n";
    }
}

void Game::handle_anticheat_message(std::string msg)
{
    using nlohmann::json;
    json json_object = json::parse(msg);

    const std::string violationMessage = "We detected an anti-cheat violation.\n"
                                         "Your access is suspended for security reasons.";

    if (json_object.is_object()) {
        // 1. Handle token request reply
        if (json_object["allowed"] == false) {
            on_violation(json_object["details"]);
            return;
        }

        // 2. Handle runtime violations
        if (json_object["status"] == "violation") {
            on_violation(json_object["details"]);
        }
    } else {
        std::cout << "Expected a json object" << std::endl;
    }
}

void Game::on_violation(std::string details)
{
    std::cout << "A violation has been detected, details: " << details << std::endl;
    m_details = details;
    state = GameState::BANNED;
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
