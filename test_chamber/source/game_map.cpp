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

#include <algorithm>
#include <iostream>
#include <fstream>

#include "game_map.h"

void GameMap::init(int w, int h)
{
    clear();

    this->w = w;
    this->h = h;

    if ((w == 0) || (h == 0)) {
        tiles = nullptr;
        return;
    }

    tiles = new int[w * h];

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            set_tile(i, j, -1);
        }
    }
}

void GameMap::clear()
{
    if (tiles != nullptr) {
        delete tiles;
        tiles = nullptr;
    }
    w = 0;
    h = 0;
}

int GameMap::get_tile(int x, int y) const
{
    int index = x + (y * w);
    if (index > (w * h)) {
        return -1;
    }

    return tiles[index];
}

bool GameMap::set_tile(int x, int y, int id)
{
    int index = x + (y * w);
    if (index >= (w * h)) {
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

void GameMap::print() const
{
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            std::cout << get_tile(x, y) << ", ";
        }

        std::cout << std::endl;
    }
}