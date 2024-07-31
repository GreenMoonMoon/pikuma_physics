//
// Created by josue on 2023-10-04.
//

#ifndef PIKUMA_PHYSICS_RAYLIB_UTILS_H
#define PIKUMA_PHYSICS_RAYLIB_UTILS_H

#include "cglm/cglm.h"

//void Init();
//void Cleanup();
//
//void ShadedCircle(const Body &body);
//void ShadedBox(const Body &body);
//
//void Circle(const Body &body);
//void drawPolygonBody(Body &body);

struct Body;
struct Texture;

void drawParticle(vec2 position, float radius, ivec4 color);
void drawPolygon(vec2 *vertices, vec2 position, float angle, ivec4 color);
void drawShadedBox(vec2 min, vec2 max, float angle, ivec4 color_a, ivec4 color_b);
void draw_box(vec2 position, float rotation, vec2 center, vec2 extents, ivec4 line_color, ivec4 fill_color);
void draw_box_line(vec2 position, float rotation, vec2 center, vec2 extents, float width, ivec4 color);
void draw_box_textured(vec2 position, float rotation, vec2 center, vec2 extents, const struct Texture *texture);
void draw_polygon(vec2 position, float rotation, vec2 *vertices, uint32_t vertex_count, ivec4 line_color, ivec4 fill_color);
void draw_polygon_line(vec2 position, float rotation, vec2 *vertices, uint32_t vertex_count, float width, ivec4 color);
void draw_circle(vec2 position, float rotation, float radius, ivec4 line_color, ivec4 fill_color);
void draw_circle_line(vec2 position, float rotation, float radius, float width, ivec4 color);
void draw_circle_textured(vec2 position, float rotation, float radius, const struct Texture *texture);

void draw_body(const struct Body *body, ivec4 fill_color, ivec4 line_color);
void draw_body_line(const struct Body *body, ivec4 color);
void draw_body_textured(const struct Body *body, const struct Texture *texture);

void draw_collision(vec2 start, vec2 end, float width, ivec4 color);

void draw_grid(int spacing, ivec4 color);

#endif //PIKUMA_PHYSICS_RAYLIB_UTILS_H
