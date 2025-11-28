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