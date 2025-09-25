#define STB_DS_IMPLEMENTATION
#include "external/stb_ds.h"

#include "raylib.h"
//#include "scenes/particles_scene.h"
#include "scenes/rigidbodies_scene.h"

static PhysicScene active_scene;

void setup(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1024, 720, "Pikuma Physics");
    SetTargetFPS(60);

    active_scene.init();
}

void update(void) {
    const float delta_time = GetFrameTime();
    active_scene.update(delta_time);
}

void render(void) {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    active_scene.render();

    DrawFPS(GetScreenWidth() - 100, 25);

    EndDrawing();
}

void cleanup(void) {
    active_scene.cleanup();

    CloseWindow();
}


int main() {
    // particles_scene_load(&scene);
    rigidbodies_scene_load(&active_scene);

    setup();
    while(!WindowShouldClose()){
        update();
        render();
    }
    cleanup();

    return 0;
}
