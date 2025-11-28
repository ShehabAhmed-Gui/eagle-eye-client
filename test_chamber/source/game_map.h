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

#ifndef _GAMEMAP_H
#define _GAMEMAP_H

#include <string>

struct GameMap
{
private:
    int* tiles = nullptr;
    int w = 0;
    int h = 0;

public:

    void init(int w, int h);
    void clear();

    inline int width() const { return w; }
    inline int height() const { return h; }

    int get_tile(int x, int y) const;
    bool set_tile(int x, int y, int id);

    void load_from_file(const std::string path);

    void print() const;
};

#endif