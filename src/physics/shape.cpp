//
// Created by josue on 2023-10-02.
//

#include "shape.h"
#include <iostream>
#include "glm/glm.hpp"

CircleShape:: CircleShape(const float radius) : Radius(radius) {}

CircleShape::~CircleShape() {
    std::cout << "Circle destructor called." << std::endl;
}

ShapeType CircleShape::GetType() const {
    return ShapeType::CIRCLE;
}

std::shared_ptr<Shape> CircleShape::Copy() const {
    return std::make_shared<CircleShape>(this->Radius);
}

float CircleShape::GetMomentOfInertia() const {
    return 0.5f * (Radius * Radius);
};

PolygonShape::PolygonShape(const std::vector<glm::vec2>& vertices) : Vertices(vertices), WorldVertices(vertices) {
    std::cout << "PolygonShape constructor called." << std::endl;
}

PolygonShape::~PolygonShape() {
    std::cout << "PolygonShape destructor called." << std::endl;
}

ShapeType PolygonShape::GetType() const {
    return ShapeType::POLYGON;
}

std::shared_ptr<Shape> PolygonShape::Copy() const {
    return std::make_shared<PolygonShape>(this->Vertices);
}

float PolygonShape::GetMomentOfInertia() const {
    return 0;
}

void PolygonShape::UpdateWorldVertices(const glm::vec2 position, const float rotation) {
    for(int i = 0; i < Vertices.size(); i++){
        WorldVertices[i].x = Vertices[i].x * glm::cos(rotation) + Vertices[i].y * glm::sin(rotation) + position.x;
        WorldVertices[i].y = Vertices[i].x * -glm::sin(rotation) + Vertices[i].y * glm::cos(rotation) + position.y;
    }
}

/// Return a polygon edge where the vertex at index is the first vertex of this edge
/// @param index edge's first vertex index
/// @return Edge direction
glm::vec2 PolygonShape::EdgeAt(const int index) const {
    const int next_vertex = (index + 1) % WorldVertices.size(); // Modulo to ensure wrap around
    return WorldVertices[next_vertex] - WorldVertices[index];
}

float PolygonShape::FindMinimumSeparation(const PolygonShape &other, glm::vec2 &axis, glm::vec2 &point) const {
    float separation = std::numeric_limits<float>::lowest();

    for (int i = 0; i < WorldVertices.size(); i++) {
        glm::vec2 va = WorldVertices[i];

        const glm::vec2 edge = EdgeAt(i);
        glm::vec2 normal(edge.y, -edge.x);
        normal = glm::normalize(normal);

        float min_sep = std::numeric_limits<float>::max();
        for (auto vb: other.WorldVertices) {
            if (const float curr_sep = glm::dot(normal, (vb - va)); curr_sep < min_sep) {
                point = vb;
                min_sep = curr_sep;
            }
        }

        if(min_sep > separation) {
            separation = min_sep;
            axis = edge;
        }
    }

    return separation;
}

BoxShape::BoxShape(const float width, const float height) : PolygonShape(std::vector<glm::vec2>()), width(width), height(height){
    Vertices.emplace_back(-0.5f * width, -0.5f * height);
    Vertices.emplace_back(0.5f * width, -0.5f * height);
    Vertices.emplace_back(0.5f * width, 0.5f * height);
    Vertices.emplace_back(-0.5f * width, 0.5f * height);

    WorldVertices = std::vector<glm::vec2>(Vertices);
}

BoxShape::~BoxShape() {
    std::cout << "Box destructor called." << std::endl;
}

ShapeType BoxShape::GetType() const {
    return ShapeType::BOX;
}

std::shared_ptr<Shape> BoxShape::Copy() const {
    return std::make_shared<BoxShape>(this->width, this->height);
}

float BoxShape::GetMomentOfInertia() const {
    constexpr float ifac = 1.0f/12.0f;
    return ifac * (width * width + height * height);
}
