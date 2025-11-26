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

#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

#include "game.h"

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

    bool on_ground;

    Color color;

    Player();
    void update(GameMap& map);
    void draw();
    void collide(GameMap& map, Vector2 velocity);

    void accelerate(int dir);
    void halt_x_movement();
};

#endif