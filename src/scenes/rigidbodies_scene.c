//
// Created by josue on 2024-07-22.
//

#include "raylib.h"
#include "rigidbodies_scene.h"
#include "physics/rigidbodies.h"
#include "physics/collision.h"
#include "physics/forces.h"
#include "external/stb_ds.h"

static Body *bodies = NULL;
static vec2 *vertices;

static bool ui_enabled = false;
static vec2 ui_window_bar[2] = {{10,10},{210, 34}};
static vec2 ui_drag_offset = {0};
static bool paused = false;
static bool enable_gravity = true;
static enum {
    NONE_MODE,
    ADD_CIRCLE_MODE,
    DRAG_DEBUG_WINDOW_MODE,
} mode = 0;
static struct SpawnInfo {
    vec2 position;
    float radius;
    float mass;
    bool set;
} spawn_info = {
        .position = {0},
        .radius = PIXEL_PER_UNIT,
        .mass = 1.0f,
        .set = false,
};

static Contact *collisions = NULL;

static Texture2D background;
static Texture2D sphere_texture;
static Texture2D square_texture;

static void handle_inputs(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { mode = NONE_MODE; }
    if (IsKeyPressed(KEY_D) && IsKeyDown(KEY_LEFT_CONTROL)) { ui_enabled = !ui_enabled; }
    if (IsKeyPressed(KEY_PAUSE)) { paused = !paused; }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        switch(mode){
            case NONE_MODE:
                if (ui_enabled && glm_aabb2d_point(ui_window_bar, (vec2) {GetMouseX(), GetMouseY()})) {
                    mode = DRAG_DEBUG_WINDOW_MODE;
                    ui_drag_offset[0] = GetMouseX() - ui_window_bar[0][0];
                    ui_drag_offset[1] = GetMouseY() - ui_window_bar[0][1];
                }
                break;
            case ADD_CIRCLE_MODE:
                spawn_info.position[0] = GetMouseX();
                spawn_info.position[1] = GetMouseY();
                spawn_info.set = true;
                break;
            default:
                break;
        }
    }else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        switch(mode){
            case ADD_CIRCLE_MODE:
                spawn_info.radius = glm_clamp(abs(GetMouseX() - (int) spawn_info.position[0]), 1.0f * PIXEL_PER_UNIT, 100.0f * PIXEL_PER_UNIT);
                spawn_info.mass = glm_clamp(abs(GetMouseY() - (int) spawn_info.position[1]) * 0.1f, 1.0f, 100.0f);
                break;
            case DRAG_DEBUG_WINDOW_MODE:
                ui_window_bar[0][0] = GetMouseX() - ui_drag_offset[0];
                ui_window_bar[0][1] = GetMouseY() - ui_drag_offset[1];
                ui_window_bar[1][0] = ui_window_bar[0][0] + 200;
                ui_window_bar[1][1] = ui_window_bar[0][1] + 24;
                break;
            default:
                break;
        }
    }else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        switch (mode) {
            case ADD_CIRCLE_MODE:
                arrput(bodies, create_circle_body(spawn_info.radius, spawn_info.mass, 0.9f, spawn_info.position));
                spawn_info.position[0] = 0;
                spawn_info.position[1] = 0;
                spawn_info.mass = 1.0f;
                spawn_info.radius = PIXEL_PER_UNIT;
                spawn_info.set = false;
                mode = NONE_MODE;
                break;
            case DRAG_DEBUG_WINDOW_MODE:
                mode = NONE_MODE;
                break;
            default:
                break;
        }
    }
}

void rigidbodies_scene_init(void) {
    background = LoadTexture("../assets/PNG/Backgrounds/blue_grass.png");
//    sphere_texture = LoadTexture("../assets/PNG/Wood elements/elementWood006.png");

//    arrput(bodies, create_circle_body(1.0f * PIXEL_PER_UNIT, 1.0f, 0.9f, (vec2) {300, 300}));
//    arrput(bodies, create_circle_body(2.0f * PIXEL_PER_UNIT, 2.0f, 0.9f, (vec2) {325, 100}));

    square_texture = LoadTexture("../assets/PNG/Wood elements/elementWood010.png");

    arrput(bodies, create_box_body((vec2){0}, (vec2){50,50}, 2.0f, 0.9f, (vec2){700, 400}));
    arrput(bodies, create_box_body((vec2){0}, (vec2){50,50}, 2.0f, 0.5f, (vec2){650, 200}));

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
    handle_inputs();

    if(mode == ADD_CIRCLE_MODE && !spawn_info.set){
        spawn_info.position[0] = GetMouseX();
        spawn_info.position[1] = GetMouseY();
    }

    if (paused) { return; }

    // clear collisions array
    arrsetlen(collisions, 0);

    for (int i = 0; i < arrlen(bodies); ++i) {
        // add forces
        vec2 forces = {0.0f, 0.0f};

        if (enable_gravity) { forces[1] = 10.0f * PIXEL_PER_UNIT; } // add gravity;

        force_apply_drag(bodies[i].linear_velocity, 0.001f, forces);

        body_integrate_linear(&bodies[i], forces, delta_time);


        // add torques
        float torques = 0.0f;
        body_integrate_angular(&bodies[i], torques, delta_time);

        // check boundary collisions
        Contact contact;
        switch (bodies[i].type) {
            case BOX_SHAPE_TYPE:
                box_check_resolve_boundary(&bodies[i], (vec2) {0}, (vec2) {GetScreenWidth(), GetScreenHeight()});
                break;
            case POLYGON_SHAPE_TYPE:
                break;
            case CIRCLE_SHAPE_TYPE:
                circle_check_resolve_boundary(&bodies[i], (vec2) {0}, (vec2) {GetScreenWidth(), GetScreenHeight()});
                for (int j = i + 1; j < arrlen(bodies); ++j) {
                    if (circle_circle_collision_check(&bodies[i], &bodies[j], &contact)) {
                        resolve_collision(contact);
                    }
                }
                break;
        }
    }
}

void rigidbodies_scene_render(void) {
    DrawTexture(background, 0, -100, WHITE);

    for (int i = 0; i < arrlen(bodies); ++i) {
        DrawRectangleLines(bodies[i].position[0] - bodies[i].box_shape.extents[0], bodies[i].position[1] - bodies[i].box_shape.extents[1], 2 * bodies[i].box_shape.extents[0], 2 * bodies[i].box_shape.extents[1], BLACK);
    }

    if(mode == ADD_CIRCLE_MODE){
        DrawCircleLines(spawn_info.position[0], spawn_info.position[1], spawn_info.radius, BLACK);
        DrawText(TextFormat("%.1f", spawn_info.mass), spawn_info.position[0], spawn_info.position[1], 18, GREEN);
    }
}

void rigidbodies_scene_cleanup(void) {
    free(vertices);
    arrfree(bodies);
    arrfree(collisions);
    UnloadTexture(background);
//    UnloadTexture(sphere_texture);
    UnloadTexture(square_texture);
}

void rigidbodies_scene_load(PhysicScene *scene) {
    scene->init = rigidbodies_scene_init;
    scene->update = rigidbodies_scene_update;
    scene->render = rigidbodies_scene_render;
    scene->cleanup = rigidbodies_scene_cleanup;
}
