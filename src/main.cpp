#include "raylib.h"
#include "vectors.h"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(840, 680, "Pikuma Physics");

    while(!WindowShouldClose()){
        PollInputEvents();

        ClearBackground(BLACK);

        BeginDrawing();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
