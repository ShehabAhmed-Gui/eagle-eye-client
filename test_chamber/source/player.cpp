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

#include "player.h"
#include "gameconst.h"

Player::Player()
{
    x = 300;
    y = 300;
    w = 50;
    h = 50;

    origin = {(float)w / 2, 0};

    speed = 5.0f;
    acceleration = 0.5f;
    friction = 0.3f;
    gravity = 0.3f;
    jump_force = 10.0f;
    velocity = {0};
    rotation = 0;
    rotation_speed = 0.8f;
    max_rotation = 2.5f;

    color = {160, 40, 40, 255};
}

void Player::update()
{
    if (on_ground()) {
        velocity.y = 0;
    }
    else {
        velocity.y += gravity;
    }

    if (IsKeyDown(KEY_D)) {
        accelerate(1);
    }
    else if (IsKeyDown(KEY_A)) {
        accelerate(-1);
    }
    else {
        halt_x_movement();
    }

    if (IsKeyPressed(KEY_SPACE) && on_ground()) {
        velocity.y = -jump_force;
    }

    x += velocity.x;
    y += velocity.y;

    collide();
}

void Player::draw()
{
    Rectangle rect = {
                      x,
                      y,
                      w,
                      h
                      };

    if (IsKeyDown(KEY_D)) {
        rotation += rotation_speed;
    }
    else if (IsKeyDown(KEY_A)) {
        rotation -= rotation_speed;
    }
    else {
        if (rotation > 0) {
            rotation -= rotation_speed;
            if (rotation < 0) {
                rotation = 0;
            }
        }
        else if (rotation < 0) {
            rotation += rotation_speed;
            if (rotation > 0) {
                rotation = 0;
            }
        }
    }

    if (rotation > max_rotation) {
        rotation = max_rotation;
    }
    else if (rotation < -max_rotation) {
        rotation = -max_rotation;
    }

    DrawRectanglePro(rect, origin, rotation, color);
}

void Player::collide()
{
    int top_left = x - origin.x;
    if ((y + h) >= WINDOW_HEIGHT) {
        y = WINDOW_HEIGHT - h;
    }
    else if (y < 0) {
        y = 0;
    }

    if ((top_left + w) >= WINDOW_WIDTH) {
        top_left = WINDOW_WIDTH - w;

        if (velocity.x > 0) {
            velocity.x = 0;
        }
    }
    else if (top_left < 0) {
        top_left = 0;

        if (velocity.x < 0) {
            velocity.x = 0;
        }
    }

    x = top_left + origin.x;
}

bool Player::on_ground()
{
    if ((y + h) >= WINDOW_HEIGHT) {
        return true;
    }

    return false;
}

void Player::accelerate(int dir)
{
    velocity.x += acceleration * dir;

    if (velocity.x > speed) {
        velocity.x = speed;
    }
    else if (velocity.x < -speed) {
        velocity.x = -speed;
    }
}

void Player::halt_x_movement()
{
    if (velocity.x < 0) {
        velocity.x += friction;

        if (velocity.x > 0) {
            velocity.x = 0;
        }
    }

    else if (velocity.x > 0) {
        velocity.x -= friction;

        if (velocity.x < 0) {
            velocity.x = 0;
        }
    }
}