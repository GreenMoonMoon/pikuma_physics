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

static bool ui_enabled = true;
static vec2 ui_window_bar[2] = {{10,10},{210, 34}};
static bool ui_dragged = false;
static vec2 ui_drag_offset = {0};
static bool paused = false;
static bool enable_gravity = true;

static Contact *collisions = NULL;

static void draw_ui(void) {
    if (GuiWindowBox((Rectangle) {ui_window_bar[0][0], ui_window_bar[0][1], 200, 100}, "Controls")) { ui_enabled = false; }
    GuiCheckBox((Rectangle) {ui_window_bar[0][0] + 15, ui_window_bar[0][1] + 38, 25, 25}, "Pause", &paused);
    GuiCheckBox((Rectangle) {ui_window_bar[0][0] + 15, ui_window_bar[0][1] + 68, 25, 25}, "Gravity", &enable_gravity);
}

void rigidbodies_scene_init(void) {
    arrput(bodies, create_circle_body(1.0f * PIXEL_PER_UNIT, 1.0f, (vec2) {300, 300}));
    arrput(bodies, create_circle_body(2.0f * PIXEL_PER_UNIT, 2.0f, (vec2) {325, 100}));

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
    if (IsKeyPressed(KEY_D) && IsKeyDown(KEY_LEFT_CONTROL)) { ui_enabled = true; }
    if (IsKeyPressed(KEY_PAUSE)) { paused = !paused; }

    // update ui window
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ui_enabled) {
        if (glm_aabb2d_point(ui_window_bar, (vec2) {GetMouseX(), GetMouseY()})) {
            ui_dragged = true;
            ui_drag_offset[0] = GetMouseX() - ui_window_bar[0][0];
            ui_drag_offset[1] = GetMouseY() - ui_window_bar[0][1];
        }
    }else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && ui_dragged){
        ui_window_bar[0][0] = GetMouseX() - ui_drag_offset[0];
        ui_window_bar[0][1] = GetMouseY() - ui_drag_offset[1];
        ui_window_bar[1][0] = ui_window_bar[0][0] + 200;
        ui_window_bar[1][1] = ui_window_bar[0][1] + 24;
    }else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        ui_dragged = false;
    }

    if (paused) { return; }

    // clear collisions array
    arrsetlen(collisions, 0);

    for (int i = 0; i < arrlen(bodies); ++i) {
        // add forces
        vec2 forces = {0.0f, 0.0f};

        if (enable_gravity) { forces[1] = 10.0f * PIXEL_PER_UNIT; } // add gravity;

        // add wind force
        add_force((vec2){10.0f * PIXEL_PER_UNIT, 0.0f}, bodies[i].inverse_mass, forces);

        apply_drag_force(bodies[i].linear_velocity, 0.001f, forces);

        body_integrate_linear(&bodies[i], forces, delta_time);


        // add torques
        float torques = 0.01f;
        body_integrate_angular(&bodies[i], torques, delta_time);

        // check boundary collisions
        circle_check_resolve_boundary(&bodies[i], (vec2) {0}, (vec2) {GetScreenWidth(), GetScreenHeight()});

        Contact contact;
        for (int j = i + 1; j < arrlen(bodies); ++j) {
            if (circle_circle_collision_check(&bodies[i], &bodies[j], &contact)) {
                arrput(collisions, contact);
                resolve_collision(contact);
            }
        }
    }
}

void rigidbodies_scene_render(void) {
    draw_grid(79);

    for (int i = 0; i < arrlen(bodies); ++i) {
        draw_body_line(&bodies[i], (ivec4) {255, 255, 255, 255});
    }

    for (int i = 0; i < arrlen(collisions); ++i) {
        draw_body_line(collisions[i].a, (ivec4) {200, 0, 0, 255});
        draw_body_line(collisions[i].b, (ivec4) {200, 0, 0, 255});

        draw_collision(collisions[i].start, collisions[i].end, 2.0f, (ivec4) {25, 25, 0, 255});
    }

    if (ui_enabled) { draw_ui(); }
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
