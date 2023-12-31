//
// Created by josue on 2023-10-02.
//

#include "shape.h"
#include <iostream>

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

PolygonShape::PolygonShape(const std::vector<glm::vec2> vertices) : Vertices(vertices) {}

PolygonShape::~PolygonShape() {
    std::cout << "Polygon destructor called." << std::endl;
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

BoxShape::BoxShape(float width, float height) : width(width), height(height), PolygonShape(std::vector<glm::vec2>()){
    Vertices.emplace_back(-0.5f * width, -0.5f * height);
    Vertices.emplace_back(0.5f * width, -0.5f * height);
    Vertices.emplace_back(0.5f * width, 0.5f * height);
    Vertices.emplace_back(-0.5f * width, 0.5f * height);
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
