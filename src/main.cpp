#include "raylib.h"
#include "particles.h"
#include "physic_constants.h"
#include <vector>

class Application {
private:
    bool running;
    std::vector<Particle> particles;

    float elapsed_time;

public:
    Application() : running(true), particles({}), elapsed_time(0.0f) {
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
        particles.push_back(Particle(50, 100, 1.0));
        particles.push_back(Particle(25, 200, 2.0));
    }

    void Input() {
        PollInputEvents();
    }

    void Update() {
        Vector2 wind = Vector2Scale(Vector2(1.0f, 0.0f), PIXEL_PER_METER);
        Vector2 gravity = Vector2Scale(Vector2(0.0f, 9.8f), PIXEL_PER_METER);
        for (Particle &particle: particles) {

            particle.AddForce(wind);
            particle.AddForce(gravity);

            particle.Integrate(GetFrameTime());

            // Boundary collisions
            if (particle.Position.x > 836.0f) {
                particle.Position.x = 836.0f;
                particle.Velocity.x *= -0.9f;
            }
            if (particle.Position.x < 4.0f) {
                particle.Position.x = 4.0f;
                particle.Velocity.x *= -0.9f;
            }
            if (particle.Position.y > 676.0f) {
                particle.Position.y = 676.0f;
                particle.Velocity.y *= -0.9f;
            }
            if (particle.Position.y < 4.0f) {
                particle.Position.y = 4.0f;
                particle.Velocity.y *= -0.9f;
            }
        }
    }

    void Render() {
        ClearBackground(DARKGREEN);

        BeginDrawing();

        for (Particle &particle: particles) {
            DrawCircle(particle.Position.x, particle.Position.y, 4.0f, WHITE);
        }

        EndDrawing();
    }

    void Cleanup() {
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
