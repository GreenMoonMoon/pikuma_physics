//
// Created by moonmoon on 30/07/23.
//
#include "vectors.h"


namespace Vectors {
    Vector2::Vector2() : X(0), Y(0) {}

    Vector2::Vector2(float X, float Y) : X(X), Y(Y) {}

    Vector2::Vector2(float v) : X(v), Y(v) {}

    void Vector2::Add(const Vector2 &vector) {
        X += vector.X;
        Y += vector.Y;
    }

    void Vector2::Subtract(const Vector2 &vector) {
        X -= vector.X;
        Y -= vector.Y;
    }

    void Vector2::Scale(const float scalar) {
        X *= scalar;
        Y *= scalar;
    }

    Vector2 Vector2::Rotate(const float angle) const {
        return {X * cosf(angle) - Y * sinf(angle), X * sinf(angle) + Y * cosf(angle)};
    }

    float Vector2::MagnitudeSquare() const {
        return X * X + Y * Y;
    }

    float Vector2::Magnitude() const {
        return sqrtf(X * X + Y * Y);
    }

    Vector2 &Vector2::Normalize() {
        float length = Magnitude();
        if (length != 0.0f) {
            X /= length;
            Y /= length;
        }
        return *this;
    }

    Vector2 Vector2::Unit() const {
        float length = Magnitude();
        Vector2 result(0.0f, 0.0f);
        if (length != 0.0f) {
            result.X = X / length;
            result.Y = Y / length;
        }
        return result;
    }

    Vector2 Vector2::Normal() const {
        return Vector2(Y, -X).Normalize();
    }

    float Vector2::Dot(const Vector2 &vector) const {
        return X * vector.X + Y * vector.Y;
    }

    float Vector2::Cross(const Vector2 &vector) const {
        return (X * vector.Y) - (Y * vector.X);
    }
}