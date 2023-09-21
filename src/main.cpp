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
        particles->Velocity = Vector2(0.0f, 0.0f);
        particles->Acceleration = Vector2(15.0f, 25.0f);
    }

    void Input() {
        PollInputEvents();
    }

    void Update() {
        // Update velocity
        particles->Velocity = Vector2Add(particles->Velocity, Vector2Scale(particles->Acceleration, GetFrameTime()));
        particles->Velocity = Vector2Add(particles->Velocity,
                                         Vector2Scale({0.0f, 9.8 * PIXEL_PER_METER}, GetFrameTime()));

        // Integrate velocity
        particles->Position = Vector2Add(particles->Position, Vector2Scale(particles->Velocity, GetFrameTime()));

        // Boundary collisions
        if (particles->Position.x > 836.0f) {
            particles->Position.x = 836.0f;
            particles->Velocity.x *= -0.9f;
        }
        if (particles->Position.x < 4.0f) {
            particles->Position.x = 4.0f;
            particles->Velocity.x *= -0.9f;
        }
        if (particles->Position.y > 676.0f) {
            particles->Position.y = 676.0f;
            particles->Velocity.y *= -0.9f;
        }
        if (particles->Position.y < 4.0f) {
            particles->Position.y = 4.0f;
            particles->Velocity.y *= -0.9f;
        }
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

    while (app.IsRunning()) {
        app.Input();
        app.Update();
        app.Render();
    }

    return 0;
}
