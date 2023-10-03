//
// Created by josue on 2023-10-02.
//

#ifndef PIKUMA_PHYSICS_SHAPE_H
#define PIKUMA_PHYSICS_SHAPE_H

#include <vector>
#include "glm/vec2.hpp"

enum ShapeType {
    CIRCLE,
    POLYGON,
    BOX,
};

struct Shape {
    virtual ~Shape() = default;
    virtual ShapeType GetType() const = 0;
    virtual Shape *Copy() const = 0;
};

struct CircleShape: public Shape {
    float radius;

    CircleShape(float radius);
    virtual ~CircleShape();
    ShapeType GetType() const override;
    Shape *Copy() const override;
};

struct PolygonShape : public Shape {
    std::vector<glm::vec2> vertices;

    PolygonShape(const std::vector<glm::vec2> vertices);
    virtual ~PolygonShape();
    ShapeType GetType() const override;
    Shape *Copy() const override;
};

struct BoxShape : public PolygonShape {
    float width;
    float height;

    BoxShape(float width, float height);
    virtual ~BoxShape();
    ShapeType GetType() const override;
    Shape *Copy() const override;
};

#endif //PIKUMA_PHYSICS_SHAPE_H
