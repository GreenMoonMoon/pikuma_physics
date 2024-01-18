//
// Created by josue on 2023-10-04.
//

#ifndef PIKUMA_PHYSICS_RAYLIB_EXTENSION_H
#define PIKUMA_PHYSICS_RAYLIB_EXTENSION_H

#include "raylib.h"
#include <vector>
#include "glm/vec2.hpp"

void DrawPolygon(std::vector<glm::vec2> &vertices, glm::vec2 position, float angle, Color color);
void draw_shaded_box(Rectangle rec, float angle, Color a, Color b);

#endif //PIKUMA_PHYSICS_RAYLIB_EXTENSION_H
