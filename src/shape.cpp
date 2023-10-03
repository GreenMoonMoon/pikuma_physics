//
// Created by josue on 2023-10-02.
//

#include "shape.h"
#include <iostream>

CircleShape::CircleShape(const float radius) : radius(radius) {}

CircleShape::~CircleShape() {
    std::cout << "Circle destructor called." << std::endl;
}

ShapeType CircleShape::GetType() const {
    return ShapeType::CIRCLE;
}

Shape *CircleShape::Copy() const {
    return new CircleShape(this->radius);
};

PolygonShape::PolygonShape(const std::vector<glm::vec2> vertices) : vertices(vertices) {}

PolygonShape::~PolygonShape() {
    std::cout << "Polygon destructor called." << std::endl;
}

ShapeType PolygonShape::GetType() const {
    return ShapeType::POLYGON;
}

Shape *PolygonShape::Copy() const {
    return new PolygonShape(this->vertices);
}

BoxShape::BoxShape(float width, float height) : width(width), height(height), PolygonShape(std::vector<glm::vec2>(4)){}

BoxShape::~BoxShape() {
    std::cout << "Box destructor called." << std::endl;
}

ShapeType BoxShape::GetType() const {
    return ShapeType::BOX;
}

Shape *BoxShape::Copy() const {
    return new BoxShape(this->width, this->height);
}
