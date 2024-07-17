//
// Created by josue on 2024-07-17.
//

#ifndef PIKUMA_PHYSICS_PARTICLES_SCENE_H
#define PIKUMA_PHYSICS_PARTICLES_SCENE_H

#include "scene.h"

void particles_scene_init(void);
void particles_scene_update(float delta_time);
void particles_scene_render();
void particles_scene_cleanup(void);

void particles_scene_load(Scene *scene);

#endif //PIKUMA_PHYSICS_PARTICLES_SCENE_H
