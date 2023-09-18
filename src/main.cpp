#include "raylib.h"
#include "particles.h"
#include "physic_constants.h"

class Application {
private:
    bool running;
    Particle *particles;
    float elapsed_time;

public:
    Application() : running(true), particles(nullptr), elapsed_time(0.0f) {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(840, 680, "Pikuma Physics");

        SetTargetFPS(60.0f);

        Setup();
    }

    ~Application() {
        Cleanup();

        CloseWindow();
    }

    inline bool IsRunning() const {
        return running and not WindowShouldClose();
    }

    void Setup() {
        particles = new Particle(50, 100, 1.0);
        particles->Velocity = Vector2(200.0f, 100.0f);
    }

    void Input() {
        PollInputEvents();
    }

    void Update() {
        particles->Position = Vector2Add(particles->Position, Vector2Scale(particles->Velocity, GetFrameTime()));
    }

    void Render() {
        ClearBackground(DARKGREEN);

        BeginDrawing();

        DrawCircle(particles->Position.x, particles->Position.y, 4.0f, WHITE);

        EndDrawing();
    }

    void Cleanup() {
        delete particles;
    }
};

int main() {
    Application app;

    while(app.IsRunning()){
        app.Input();
        app.Update();
        app.Render();
    }

    return  0;
}
