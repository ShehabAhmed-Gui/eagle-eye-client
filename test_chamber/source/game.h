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

#ifndef GAME_H
#define GAME_H

#include "cheat.h"
#include "game_map.h"
#include "player.h"

#include <raylib.h>

#include <string>


class Game
{
    enum GameState
    {
        PLAY,
        BANNED
    };

private:
    GameState state;
    EagleEye::Connection ac_connection;

    void init_anticheat();
    void handle_anticheat_message(std::string msg);
    void on_violation(std::string details);

public:
    GameMap map;
    Texture2D background;
    Player player;

    Game();
    ~Game();

    void loop();
    void update();
    void draw();
    void draw_centered_text(const char* text, float y_factor, int font_size, Color color);
    void draw_map();

private:
    std::string m_violation_details;
};

#endif
