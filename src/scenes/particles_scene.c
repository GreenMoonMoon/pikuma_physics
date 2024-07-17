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

#define PARTICLE_COUNT 100

static Particle *particles = NULL;

void particles_scene_init(void) {
    random_seed(time(NULL) % 41328749032178);

    for (int j = 0; j < PARTICLE_COUNT; ++j) {
        arrput(particles, particle((vec2){random_u32() % 974 + 50, random_u32() % 670 + 50}, 1.0f, 1.0f));
    }
}

void particles_scene_update(float delta_time) {

}

void particles_scene_render() {
    for (int j = 0; j < arrlen(particles); ++j) {
        drawParticle(particles[j].position, 10, (ivec4){255,255,255,255});
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