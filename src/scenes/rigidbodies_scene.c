//
// Created by josue on 2024-07-22.
//

#include "raylib.h"
#include "raymath.h"
#include "rigidbodies_scene.h"
#include "../physics/rigidbodies.h"
#include "../physics/collision.h"
#include "../physics/forces.h"
#include "../extern/stb_ds.h"

static Body *bodies = NULL;

static bool paused = false;
static bool enable_gravity = true;
static enum {
    NONE_MODE,
    ADD_CIRCLE_MODE,
    DRAG_DEBUG_WINDOW_MODE,
} mode = 0;
static struct SpawnInfo {
    Vector2 position;
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
// static Texture2D sphere_texture;
// static Texture2D square_texture;

static void handle_inputs(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { mode = NONE_MODE; }
    if (IsKeyPressed(KEY_N)) { mode = ADD_CIRCLE_MODE; }
    if (IsKeyPressed(KEY_PAUSE)) { paused = !paused; }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        switch(mode){
            case NONE_MODE:
                break;
            case ADD_CIRCLE_MODE:
                spawn_info.position = GetMousePosition();
                spawn_info.set = true;
                break;
            default:
                break;
        }
    }else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        switch(mode){
            case ADD_CIRCLE_MODE:
                spawn_info.radius = Clamp(fabsf(GetMouseX() - spawn_info.position.x), 1.0f * PIXEL_PER_UNIT, 100.0f * PIXEL_PER_UNIT);
                spawn_info.mass = Clamp(fabsf(GetMouseY() - spawn_info.position.y) * 0.1f, 1.0f * PIXEL_PER_UNIT, 100.0f * PIXEL_PER_UNIT);
                break;
            default:
                break;
        }
    }else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        switch (mode) {
            case ADD_CIRCLE_MODE:
                arrput(bodies, create_circle_body(spawn_info.radius, spawn_info.mass, 0.9f, spawn_info.position));
                spawn_info.position = (Vector2){0};
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
    // sphere_texture = LoadTexture("../assets/PNG/Wood elements/elementWood006.png");

    arrput(bodies, create_circle_body(1.0f * PIXEL_PER_UNIT, 1.0f, 0.9f, (Vector2){300, 300}));
    arrput(bodies, create_circle_body(2.0f * PIXEL_PER_UNIT, 2.0f, 0.9f, (Vector2){325, 100}));

    // square_texture = LoadTexture("../assets/PNG/Wood elements/elementWood010.png");

    // arrput(bodies, create_box_body((vec2){0}, (vec2){50,50}, 2.0f, 0.9f, (vec2){700, 400}));
    // arrput(bodies, create_box_body((vec2){0}, (vec2){50,50}, 2.0f, 0.5f, (vec2){650, 200}));
}

void rigidbodies_scene_update(const float delta_time) {
    handle_inputs();

    if(mode == ADD_CIRCLE_MODE && !spawn_info.set){
        spawn_info.position = GetMousePosition();
    }

    if (paused) { return; }

    // resolve previous frame contact
    while (arrlen(collisions) > 0) {
        const Contact contact = arrpop(collisions);
        resolve_collision(contact);
    }

    // Apply forces and check boundary collisions
    for (int i = 0; i < arrlen(bodies); ++i) {
        // add forces
        Vector2 forces = {0.0f, 0.0f};

        if (enable_gravity) { forces.y = 10.0f * PIXEL_PER_UNIT; } // add gravity;
        force_apply_drag(bodies[i].linear_velocity, 0.001f, &forces);

        // Integrate  forces
        body_integrate_linear(&bodies[i], forces, delta_time);


        // add torques
        float torques = 0.0f;

        // integrate torques
        body_integrate_angular(&bodies[i], torques, delta_time);

        // check boundary collisions
        Contact contact;
        switch (bodies[i].type) {
            case BOX_SHAPE_TYPE:
                box_check_resolve_boundary(&bodies[i], (Vector2){0}, (Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()});
                break;
            case POLYGON_SHAPE_TYPE:
                break;
            case CIRCLE_SHAPE_TYPE:
                circle_check_resolve_boundary(&bodies[i], (Vector2){0}, (Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()});
                break;
        }
    }

    // Check for collisions
    for (int i = 0; i < arrlen(bodies) - 1; ++i) {
        for (int j = i + 1; j < arrlen(bodies); ++j) {
            Contact contact;
            switch (bodies[i].type) {
                case BOX_SHAPE_TYPE:
                    switch (bodies[j].type) {
                        case BOX_SHAPE_TYPE:
                            break;
                        case POLYGON_SHAPE_TYPE:
                            break;
                        case CIRCLE_SHAPE_TYPE:
                            break;
                    }
                    break;
                case POLYGON_SHAPE_TYPE:
                    switch (bodies[j].type) {
                        case BOX_SHAPE_TYPE:
                                break;
                        case POLYGON_SHAPE_TYPE:
                                break;
                        case CIRCLE_SHAPE_TYPE:
                                break;
                    }
                    break;
                case CIRCLE_SHAPE_TYPE:
                    switch (bodies[j].type) {
                        case BOX_SHAPE_TYPE:
                                break;
                        case POLYGON_SHAPE_TYPE:
                                break;
                        case CIRCLE_SHAPE_TYPE:
                            if (circle_circle_collision_check(&bodies[i], &bodies[j], &contact)) { arrput(collisions, contact); } break;
                    }
                    break;
            }
        }
    }
}

void rigidbodies_scene_render(void) {
    DrawTexture(background, 0, -100, WHITE);

    for (int i = 0; i < arrlen(bodies); ++i) {
        switch (bodies[i].type) {
            case BOX_SHAPE_TYPE:
                DrawRectangleLines(bodies[i].position.x - bodies[i].box_shape.extents.x, bodies[i].position.y - bodies[i].box_shape.extents.y, 2 * bodies[i].box_shape.extents.x, 2 * bodies[i].box_shape.extents.y, BLACK);
                break;
            case POLYGON_SHAPE_TYPE:
                DrawLineStrip((const Vector2 *)bodies[i].polygon_shape.vertices, bodies[i].polygon_shape.vertex_count, BLACK);
                break;
            case CIRCLE_SHAPE_TYPE:
                DrawCircleLines(bodies[i].position.x, bodies[i].position.y, bodies[i].circle_shape.radius, BLACK);
                DrawLine(bodies[i].position.x, bodies[i].position.y, bodies[i].position.x + cosf(bodies[i].rotation) * bodies[i].circle_shape.radius, bodies[i].position.y + sinf(bodies[i].rotation) * bodies[i].circle_shape.radius, BLACK);
                break;
        }
    }

    if(mode == ADD_CIRCLE_MODE){
        DrawCircleLines(spawn_info.position.x, spawn_info.position.y, spawn_info.radius, BLACK);
        DrawText(TextFormat("%.1f", spawn_info.mass), spawn_info.position.x, spawn_info.position.y, 18, GREEN);
    }

    // draw debug
    for (int i = 0; i < arrlen(collisions); ++i) {
        DrawRectangle(collisions[i].start.x - 4, collisions[i].start.y - 4, 8, 8, ORANGE);
        DrawLine(collisions[i].start.x, collisions[i].start.y, collisions[i].start.x + (15 * collisions[i].normal.x), collisions[i].start.y + (15 * collisions[i].normal.y), ORANGE);
    }

    // UI
    DrawText(TextFormat("Collision count: %d", arrlen(collisions)), 25, 25, 20, DARKGREEN);
}

void rigidbodies_scene_cleanup(void) {
    arrfree(bodies);
    arrfree(collisions);
    UnloadTexture(background);
    // UnloadTexture(sphere_texture);
    // UnloadTexture(square_texture);
}

void rigidbodies_scene_load(PhysicScene *scene) {
    scene->init = rigidbodies_scene_init;
    scene->update = rigidbodies_scene_update;
    scene->render = rigidbodies_scene_render;
    scene->cleanup = rigidbodies_scene_cleanup;
}
