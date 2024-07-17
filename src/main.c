#include "cglm/cglm.h"
#include "external/stb_ds.h"

#include "raylib.h"
#include "scenes/particles_scene.h"

static Scene scene;

void setup(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1080, 720, "Pikuma Physics");
    SetTargetFPS(60);

    scene.init();
}

void update(void) {
    float delta_time = GetFrameTime();
    scene.update(delta_time);
}

void render(void) {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    scene.render();

    DrawFPS(10, 10);

    EndDrawing();
}

void cleanup(void) {
    scene.cleanup();

    CloseWindow();
}


int main() {
    particles_scene_load(&scene);

    setup();
    while(!WindowShouldClose()){
        update();
        render();
    }
    cleanup();

    return 0;
}
