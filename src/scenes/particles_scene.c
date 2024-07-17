//
// Created by josue on 2024-07-17.
//

#include <time.h>

#include "particles_scene.h"
#include "physics/particles.h"
#define RANDOM_IMPLEMENTATION
#include "utils/random.h"
#include "graphics/raylib_utils.h"
#define STB_DS_IMPLEMENTATION
#include "external/stb_ds.h"

#include "cglm/cglm.h"

#define PARTICLE_COUNT 100
#define PIXEL_PER_UNIT 25

static Particle *particles = NULL;

void add_push(vec2 wind, float inverse_mass, vec2 out_forces) {

    glm_vec2_add(out_forces, (vec2){wind[0] * inverse_mass, wind[1] * inverse_mass}, out_forces);
}

void apply_drag(const vec2 velocity, float coefficient, vec2 forces) {
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

    for (int j = 0; j < PARTICLE_COUNT; ++j) {
        float mass = random_float_range(1.0f, 4.0f);
        arrput(particles, particle(
            (vec2) {random_u32() % 974 + 50, random_u32() % 670 + 50},
            mass * 4.0f,
            mass
        ));
    }
}

void particles_scene_update(float delta_time) {
    for (int j = 0; j < arrlen(particles); ++j) {
        vec2 forces = {0.0f, 10.0f}; // initialize with gravity;

        add_push((vec2){10.0f, 0.0f}, particles[j].inverse_mass, forces);

        apply_drag(particles[j].velocity, 0.003f, forces);

        // integrate forces
        vec2 acceleration = {0};
        glm_vec2_add(acceleration, forces, acceleration);
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
        drawParticle(particles[j].position, particles[j].radius, (ivec4){255,255,255,255});
    }
}

void particles_scene_cleanup(void) {
    arrfree(particles);
}

void particles_scene_load(Scene *scene) {
    scene->init = particles_scene_init;
    scene->update = particles_scene_update;
    scene->render = particles_scene_render;
    scene->cleanup = particles_scene_cleanup;
}