//
// Created by josue on 2024-07-22.
//

#include "rigidbodies_scene.h"
#include "physics/rigidbodies.h"

#include "external/stb_ds.h"

#include "graphics/raylib_utils.h"

static Body *bodies = NULL;
static vec2 *vertices;

static void draw_ui(void){

}

void rigidbodies_scene_init(void) {
    arrput(bodies, create_circle_body(1.0f * PIXEL_PER_UNIT, 1.0f, (vec2){300, 300}));
    arrput(bodies, create_box_body((vec2){0}, (vec2){100,100}, 2.0f, (vec2){700, 400}));

    vertices = malloc(sizeof(vec2) * 5);
    glm_vec2_copy((vec2){-40, 0}, vertices[0]);
    glm_vec2_copy((vec2){0, 50}, vertices[1]);
    glm_vec2_copy((vec2){40, 2}, vertices[2]);
    glm_vec2_copy((vec2){40, -2}, vertices[3]);
    glm_vec2_copy((vec2){0, -10}, vertices[4]);

    arrput(bodies, create_polygon_body(vertices, 5, 4.0f, (vec2){200, 600}));
}

void rigidbodies_scene_update(float delta_time) {
    for (int i = 0; i < arrlen(bodies); ++i) {

    }
}

void rigidbodies_scene_render(void) {
    draw_grid(79);

    for (int i = 0; i < arrlen(bodies); ++i) {
        switch (bodies[i].type) {
            case BOX_SHAPE_TYPE:
                draw_box(bodies[i].position, bodies[i].box_shape.center, bodies[i].box_shape.extents, (ivec4){0,0,0,255}, (ivec4){255,200,200,255});
                break;
            case POLYGON_SHAPE_TYPE:
                draw_polygon(bodies[i].position, bodies[i].polygon_shape.vertices, bodies[i].polygon_shape.vertex_count, (ivec4){0,0,0,255}, (ivec4){200,200,255,255});
                break;
            case CIRCLE_SHAPE_TYPE:
                draw_circle(bodies[i].position, bodies[i].circle_shape.radius, (ivec4){0,0,0,255}, (ivec4){200,255,200,255});
                break;
        }
    }

    draw_ui();
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
