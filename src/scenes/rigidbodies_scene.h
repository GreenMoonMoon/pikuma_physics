//
// Created by josue on 2024-07-22.
//

#ifndef PIKUMA_PHYSICS_RIGIDBODIES_SCENE_H
#define PIKUMA_PHYSICS_RIGIDBODIES_SCENE_H

#include "scene.h"

void rigidbodies_scene_init(void);
void rigidbodies_scene_update(float delta_time);
void rigidbodies_scene_render(void);
void rigidbodies_scene_cleanup(void);
void rigidbodies_scene_load(PhysicScene *scene);

#endif //PIKUMA_PHYSICS_RIGIDBODIES_SCENE_H
