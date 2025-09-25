//
// Created by josue on 2024-07-17.
//

#include <time.h>

#include "particles_scene.h"
#include "physics/particles.h"
#define RANDOM_IMPLEMENTATION
#include "utils/random.h"
#include "external/stb_ds.h"

#include "cglm/cglm.h"

#include "raylib.h"

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
        vec2 values;
        float value;
    };
} Force;
static Force *force_list = NULL;

static void add_push(vec2 wind, float inverse_mass, vec2 out_forces) {
    glm_vec2_add(out_forces, (vec2){wind[0] * inverse_mass, wind[1] * inverse_mass}, out_forces);
}

static void add_friction(vec2 velocity, float coefficient, vec2 normal, vec2 out_forces){
    vec2 friction_force;
    vec2 direction;
    glm_vec2_normalize_to(velocity, direction);
    glm_vec2_negate(direction);
    glm_vec2_scale(direction, coefficient, friction_force);
    glm_vec2_add(out_forces, friction_force, out_forces);
}

static void add_spring() {

}

static void apply_drag(const vec2 velocity, float coefficient, vec2 forces) {
    vec2 drag = {0};
    vec2 direction;
    float velocity_magnitude_squared = glm_vec2_norm2(velocity);
    if(velocity_magnitude_squared > 0.0f) {
        glm_vec2_normalize_to(velocity, direction);
        glm_vec2_negate(direction);
        float magnitude = coefficient * velocity_magnitude_squared;
        glm_vec2_scale(direction, magnitude, drag);
    }
    glm_vec2_add(forces, drag, forces);
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
                float mass = random_float_range(1.0f, 4.0f);
                arrput(particles, particle(
                        (vec2){GetMouseX(), GetMouseY()},
                    mass * 4.0f,
                    mass,
                    false
                ));
                break;
            case MODE_DRAW_CHAIN:
                mouse_travel = 0.0f;
                arrput(particles, particle(
                    (vec2){GetMouseX(), GetMouseY()},
                    4.0f,
                    1.0f,
                    true
                ));
                break;
        }
    } else if(IsMouseButtonDown(0) && active_mode == MODE_DRAW_CHAIN){
        mouse_travel += glm_vec2_norm((vec2){GetMouseDelta().x, GetMouseDelta().y});
        if (mouse_travel > chain_spacing) {
            mouse_travel -= chain_spacing;
            arrput(particles, particle(
                (vec2){GetMouseX(), GetMouseY()},
                4.0f,
                1.0f,
                false
            ));
        }
    }
}

void particles_scene_update(float delta_time) {
    process_inputs();

    for (int j = 0; j < arrlen(particles); ++j) {
        if (particles[j].anchor) { continue; }

        vec2 force_sum = {0};

        if (is_gravity_enabled) { force_sum[1] = 10.0f; }

        for (int i = 0; i < arrlen(force_list); ++i) {
            switch (force_list[i].type) {
                case FORCE_TYPE_WIND:
                    add_push(force_list[i].values, particles[j].inverse_mass, force_sum);
                    break;
                case FORCE_TYPE_FRICTION:
                    add_friction(particles[j].velocity, force_list[i].value, (vec2){0.0f, -1.0f},force_sum);
                    break;
            }
        }

        if (is_drag_enabled){apply_drag(particles[j].velocity, 0.003f, force_sum);}

        // integrate forces
        vec2 acceleration = {0};
        glm_vec2_add(acceleration, force_sum, acceleration);
        glm_vec2_scale(acceleration, delta_time, acceleration);
        glm_vec2_add(particles[j].velocity, acceleration, particles[j].velocity);

        // integrate velocity
        vec2 delta_velocity;
        glm_vec2_scale(particles[j].velocity, delta_time * PIXEL_PER_UNIT, delta_velocity);
        glm_vec2_add(particles[j].position, delta_velocity, particles[j].position);

        // check screen bounds
        if(particles[j].position[0] < particles[j].radius) {
            particles[j].position[0] = particles[j].radius;
            particles[j].velocity[0] = -particles[j].velocity[0];
        } else if (particles[j].position[0] > 1024 - particles[j].radius){
            particles[j].position[0] = 1024 - particles[j].radius;
            particles[j].velocity[0] = -particles[j].velocity[0];
        }
        if(particles[j].position[1] < particles[j].radius) {
            particles[j].position[1] = particles[j].radius;
            particles[j].velocity[1] = -particles[j].velocity[1];
        } else if (particles[j].position[1] > 720 - particles[j].radius){
            particles[j].position[1] = 720 - particles[j].radius;
            particles[j].velocity[1] = -particles[j].velocity[1];
        }
    }
}

void particles_scene_render() {
    for (int j = 0; j < arrlen(particles); ++j) {
        DrawRectangle(particles[j].position[0], particles[j].position[1], particles[j].radius * 2, particles[j].radius * 2, WHITE);
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