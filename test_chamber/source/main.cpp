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

#include <iostream>
#include <raylib.h>

#include "cheat.h"
#include "gameconst.h"
#include "game.h"

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TestChamber");
    SetTargetFPS(TARGET_FPS);

    if (EagleEye::is_anticheat_running()) {
        std::cout << "INFO: Anticheat service is running.\n";

        EagleEye::Connection connection = EagleEye::Connection();
        if (connection.send_token_request() == true) {
            std::cout << "INFO: Sent token request to anticheat service.\n";
        }
    }
    else {
        std::cout << "INFO: Anticheat service is NOT running.\n";
    }
    game_loop();

    CloseWindow();

    return 0;
}

