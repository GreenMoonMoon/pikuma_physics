//
// Created by josue on 2024-07-22.
//

#include "rigidbodies_scene.h"
#include "physics/rigidbodies.h"
#include "physics/collision.h"
#include "physics/forces.h"

#include "external/stb_ds.h"

#include "graphics/raylib_utils.h"

#include "external/raygui.h"

const float TWO_PI = GLM_PIf * 2.0f;

static Body *bodies = NULL;
static vec2 *vertices;

static bool show_ui = true;
static bool paused = false;
static bool enable_gravity = true;

static Collision *collisions = NULL;

static void draw_ui(void){
    if(GuiWindowBox((Rectangle){10, 10, 200, 100}, "Controls")) {
        show_ui = false;
    }
    GuiCheckBox((Rectangle){15,38,25,25}, "Pause", &paused);
    GuiCheckBox((Rectangle){15,68,25,25}, "Gravity", &enable_gravity);
}

void rigidbodies_scene_init(void) {
    arrput(bodies, create_circle_body(1.0f * PIXEL_PER_UNIT, 1.0f, (vec2){300, 300}));
    arrput(bodies, create_circle_body(2.0f * PIXEL_PER_UNIT, 2.0f, (vec2){325, 100}));

//    arrput(bodies, create_box_body((vec2){0}, (vec2){100,100}, 2.0f, (vec2){700, 400}));
//
//    vertices = malloc(sizeof(vec2) * 5);
//    glm_vec2_copy((vec2){-40, 0}, vertices[0]);
//    glm_vec2_copy((vec2){0, 50}, vertices[1]);
//    glm_vec2_copy((vec2){40, 2}, vertices[2]);
//    glm_vec2_copy((vec2){40, -2}, vertices[3]);
//    glm_vec2_copy((vec2){0, -10}, vertices[4]);
//
//    arrput(bodies, create_polygon_body(vertices, 5, 4.0f, (vec2){200, 600}));
}

void rigidbodies_scene_update(float delta_time) {
    if(IsKeyPressed(KEY_D) && IsKeyDown(KEY_LEFT_CONTROL)) {
        show_ui = true;
    }
    if(IsKeyPressed(KEY_PAUSE)) {
        paused = !paused;
    }

    if (paused) { return; }
    // clear collisions array
    arrsetlen(collisions, 0);

    for (int i = 0; i < arrlen(bodies); ++i) {
        // add forces
        vec2 forces = {0.0f, 0.0f};
        apply_drag_force(bodies[i].linear_velocity, 0.001f, forces);
        if (enable_gravity) { forces[1] = 10.0f * PIXEL_PER_UNIT; } // add gravity;
        body_integrate_linear(&bodies[i], forces, delta_time);

        // add torques
        float torques = 0.01f;
        body_integrate_angular(&bodies[i], torques, delta_time);

        // check boundary collisions
        circle_check_resolve_boundary(&bodies[i], (vec2){0}, (vec2){GetScreenWidth(), GetScreenHeight()});

        for (int j = i + 1; j < arrlen(bodies); ++j) {
            if(circle_circle_collision_check(&bodies[i], &bodies[j])) {
//                printf("collides!\n");
                // retrieve collision information
                Collision collision = circle_circle_get_collision_info(&bodies[i], &bodies[j]);
                arrput(collisions, collision);
                // resolve collisions
                collision.a->position[0] += collision.normal[0] * collision.depth * -0.5f;
                collision.a->position[1] += collision.normal[1] * collision.depth * -0.5f;
                collision.b->position[0] += collision.normal[0] * collision.depth * 0.5f;
                collision.b->position[1] += collision.normal[1] * collision.depth * 0.5f;

                glm_vec2_reflect(collision.a->linear_velocity, collision.normal, collision.a->linear_velocity);
                glm_vec2_reflect(collision.b->linear_velocity, (vec2){-collision.normal[0], -collision.normal[1]}, collision.b->linear_velocity);
            }
        }
    }
}

void rigidbodies_scene_render(void) {
    draw_grid(79);

    for (int i = 0; i < arrlen(bodies); ++i) {
        draw_body_line(&bodies[i], (ivec4){255, 255, 255, 255});
    }

    for (int i = 0; i < arrlen(collisions); ++i) {
        draw_body_line(collisions[i].a, (ivec4){200, 0, 0, 255});
        draw_body_line(collisions[i].b, (ivec4){200, 0, 0, 255});

        draw_collision(collisions[i].start, collisions[i].end, 2.0f, (ivec4){25, 25, 0, 255});
    }

    if (show_ui) { draw_ui(); }
}

void rigidbodies_scene_cleanup(void) {
    free(vertices);
    arrfree(bodies);
}

void rigidbodies_scene_load(Scene *scene) {
    scene->init = rigidbodies_scene_init;
    scene->update = rigidbodies_scene_update;
    scene->render = rigidbodies_scene_render;
    scene->cleanup = rigidbodies_scene_cleanup;
}
