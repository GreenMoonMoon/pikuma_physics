//
// Created by josue on 2024-07-17.
//

#ifndef PIKUMA_PHYSICS_SCENE_H
#define PIKUMA_PHYSICS_SCENE_H

#define PIXEL_PER_UNIT 25

typedef struct PhysicScene {
    void (*init)(void);
    void (*update)(float delta_time);
    void (*render)(void);
    void (*cleanup)(void);
} PhysicScene;

#endif //PIKUMA_PHYSICS_SCENE_H
