//
// Created by josue on 2023-10-02.
//

#ifndef PIKUMA_PHYSICS_SHAPE_H
#define PIKUMA_PHYSICS_SHAPE_H

#include <vector>
#include <memory>
#include "glm/vec2.hpp"

enum ShapeType {
    CIRCLE,
    POLYGON,
    BOX,
};

struct Shape {
    virtual ~Shape() = default;
    virtual ShapeType GetType() const = 0;
    virtual std::shared_ptr<Shape> Copy() const = 0;
    virtual float GetMomentOfInertia() const = 0;
};

struct CircleShape: public Shape {
    float Radius;

    CircleShape(float radius);
    virtual ~CircleShape();
    ShapeType GetType() const override;
    std::shared_ptr<Shape> Copy() const override;
    /// Return the moment of inertia for a solid cirle of mass 1.0
    ///  This value should be multiplied by the body's actual mass.
    /// \return Moment of inertia
    float GetMomentOfInertia() const override;
};

struct PolygonShape : public Shape {
    std::vector<glm::vec2> Vertices;

    PolygonShape(const std::vector<glm::vec2> vertices);
    virtual ~PolygonShape();
    ShapeType GetType() const override;
    std::shared_ptr<Shape> Copy() const override;
    float GetMomentOfInertia() const override;

    float FindMinimumSeparation(const PolygonShape *other);
};

struct BoxShape : public PolygonShape {
    float width;
    float height;

    BoxShape(float width, float height);
    virtual ~BoxShape();
    ShapeType GetType() const override;
    std::shared_ptr<Shape> Copy() const override;
    /// Return the moment of inertia for a solid 2D box of mass 1.0
    ///  This value should be multiplied by the body's actual mass.
    /// \return Moment of inertia
    float GetMomentOfInertia() const override;
};

#endif //PIKUMA_PHYSICS_SHAPE_H
