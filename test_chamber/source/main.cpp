#include <raylib.h>

#include "gameconst.h"
#include "player.h"

void game_loop();

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TestChamber");
    
    SetTargetFPS(TARGET_FPS);
    game_loop();

    CloseWindow();

    return 0;
}

void game_loop()
{
    Player player = Player();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        player.update();
        player.draw();

        EndDrawing();
    }
}

