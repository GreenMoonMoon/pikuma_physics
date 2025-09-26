//
// Created by josue on 2024-07-17.
//

#include <time.h>
#include "raylib.h"
#include "raymath.h"
#include "particles_scene.h"
#include "../physics/particles.h"
#define RANDOM_IMPLEMENTATION
#include "../utils/random.h"
#include "../extern/stb_ds.h"

#define PARTICLE_COUNT 100

static Particle *particles = NULL;

// Mode selection
enum {
    MODE_ADD_PARTICLE = 0,
    MODE_DRAW_CHAIN,
} active_mode = MODE_ADD_PARTICLE;
bool mode_is_edited = false;

// Force list
int focus = 0, scroll = 0; // Needed by GuiDMPropertyList()

// GUI states
static float mouse_travel;
static bool is_gravity_enabled = true;
static bool is_drag_enabled = true;
static float chain_spacing = 100.0f;

// Forces
typedef struct Force {
    enum ForceType {
        FORCE_TYPE_WIND,
        FORCE_TYPE_FRICTION,
    } type;
    union{
        Vector2 values;
        float value;
    };
} Force;
static Force *force_list = NULL;

static void add_push(const Vector2 wind, const float inverse_mass, Vector2 *out_forces) {
    *out_forces = Vector2Add(*out_forces, (Vector2){wind.x * inverse_mass, wind.y * inverse_mass});
}

static void add_friction(const Vector2 velocity, const float coefficient, Vector2 *out_forces){
    const Vector2 direction = Vector2Negate(Vector2Normalize(velocity));
    const Vector2 friction_force = Vector2Scale(direction, coefficient);
    *out_forces = Vector2Add(*out_forces, friction_force);
}

static void add_spring() {

}

static void apply_drag(const Vector2 velocity, const float coefficient, Vector2 *forces) {
    Vector2 drag = {0};
    const float velocity_magnitude_squared = Vector2LengthSqr(velocity);
    if(velocity_magnitude_squared > 0.0f) {
        const Vector2 direction = Vector2Negate(Vector2Normalize(velocity));
        const float magnitude = coefficient * velocity_magnitude_squared;
        drag = Vector2Scale(direction, magnitude);
    }
    *forces = Vector2Add(*forces, drag);
}

void particles_scene_init(void) {
    random_seed(time(NULL) % 41328749032178);

//    for (int j = 0; j < PARTICLE_COUNT; ++j) {
//        float mass = random_float_range(1.0f, 4.0f);
//        arrput(particles, particle(
//            (vec2) {random_u32() % 974 + 50, random_u32() % 670 + 50},
//            mass * 4.0f,
//            mass,
//            false
//        ));
//    }

//    // Add forces
//    Force force = {
//        .type = FORCE_TYPE_WIND,
//        .values = {10.0f, 0.0f},
//    };
//    arrput(force_list, force);
//    force.type = FORCE_TYPE_FRICTION;
//    force.value = 0.0f;
//    arrput(force_list, force);

    active_mode = MODE_DRAW_CHAIN;
}

void process_inputs(void) {
    if (IsMouseButtonPressed(0) && !mode_is_edited) {
        switch (active_mode) {
            case MODE_ADD_PARTICLE:
                const float mass = random_float_range(1.0f, 4.0f);
                arrput(particles, particle( GetMousePosition(), mass * 4.0f, mass, false));
                break;
            case MODE_DRAW_CHAIN:
                mouse_travel = 0.0f;
                arrput(particles, particle( GetMousePosition(), 4.0f, 1.0f, true));
                break;
        }
    } else if(IsMouseButtonDown(0) && active_mode == MODE_DRAW_CHAIN){
        mouse_travel += Vector2Length(GetMouseDelta());
        if (mouse_travel > chain_spacing) {
            mouse_travel -= chain_spacing;
            arrput(particles, particle( GetMousePosition(), 4.0f, 1.0f, false));
        }
    }
}

void particles_scene_update(float delta_time) {
    process_inputs();

    for (int j = 0; j < arrlen(particles); ++j) {
        if (particles[j].anchor) { continue; }

        Vector2 force_sum = {0};

        if (is_gravity_enabled) { force_sum.y = 10.0f; }

        for (int i = 0; i < arrlen(force_list); ++i) {
            switch (force_list[i].type) {
                case FORCE_TYPE_WIND:
                    add_push(force_list[i].values, particles[j].inverse_mass, &force_sum);
                    break;
                case FORCE_TYPE_FRICTION:
                    add_friction(particles[j].velocity, force_list[i].value, &force_sum);
                    break;
            }
        }

        if (is_drag_enabled){apply_drag(particles[j].velocity, 0.003f, &force_sum);}

        // integrate forces
        Vector2 acceleration = force_sum;
        acceleration = Vector2Scale(acceleration, delta_time);
        particles[j].velocity = Vector2Add(particles[j].velocity, acceleration);

        // integrate velocity
        const Vector2 delta_velocity = Vector2Scale(particles[j].velocity, delta_time * PIXEL_PER_UNIT);
        particles[j].position = Vector2Add(particles[j].position, delta_velocity);

        // check screen bounds
        if(particles[j].position.x < particles[j].radius) {
            particles[j].position.x = particles[j].radius;
            particles[j].velocity.x = -particles[j].velocity.x;
        } else if (particles[j].position.x > 1024 - particles[j].radius){
            particles[j].position.x = 1024 - particles[j].radius;
            particles[j].velocity.x = -particles[j].velocity.x;
        }
        if(particles[j].position.y < particles[j].radius) {
            particles[j].position.y = particles[j].radius;
            particles[j].velocity.y = -particles[j].velocity.y;
        } else if (particles[j].position.y > 720 - particles[j].radius){
            particles[j].position.y = 720 - particles[j].radius;
            particles[j].velocity.y = -particles[j].velocity.y;
        }
    }
}

void particles_scene_render() {
    for (int j = 0; j < arrlen(particles); ++j) {
        DrawRectangle(particles[j].position.x, particles[j].position.y, particles[j].radius * 2, particles[j].radius * 2, WHITE);
    }
}

void particles_scene_cleanup(void) {
    arrfree(particles);
    arrfree(force_list);
}

void particles_scene_load(PhysicScene *scene) {
    scene->init = particles_scene_init;
    scene->update = particles_scene_update;
    scene->render = particles_scene_render;
    scene->cleanup = particles_scene_cleanup;
}