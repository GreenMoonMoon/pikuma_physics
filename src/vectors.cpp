//
// Created by moonmoon on 30/07/23.
//
#include "vectors.h"

Vectors::Vector2::Vector2() : X(0), Y(0) {}

Vectors::Vector2::Vector2(float X, float Y) : X(X), Y(Y) {}

void Vectors::Vector2::Add(const Vectors::Vector2 &vector) {
    X += vector.X;
    Y += vector.Y;
}

void Vectors::Vector2::Subtract(const Vectors::Vector2 &vector) {
    X -= vector.X;
    Y -= vector.Y;
}

void Vectors::Vector2::Scale(const float scalar) {
    X *= scalar;
    Y *= scalar;
}

Vectors::Vector2 Vectors::Vector2::Rotate(const float angle) const {
    return {X * cosf(angle) - Y * sinf(angle), X * sinf(angle) + Y * cosf(angle)};
}

float Vectors::Vector2::MagnitudeSquare() const {
    return X * X + Y * Y;
}

float Vectors::Vector2::Magnitude() const {
    return sqrtf(X * X + Y * Y);
}

Vectors::Vector2 &Vectors::Vector2::Normalize() {
    float length = Magnitude();
    if (length != 0.0f) {
        X /= length;
        Y /= length;
    }
    return *this;
}

Vectors::Vector2 Vectors::Vector2::Unit() const {
    float length = Magnitude();
    Vector2 result(0.0f, 0.0f);
    if (length != 0.0f) {
        result.X = X / length;
        result.Y = Y / length;
    }
    return result;
}

Vectors::Vector2 Vectors::Vector2::Normal() const {
    return Vector2(Y, -X).Normalize();
}

float Vectors::Vector2::Dot(const Vectors::Vector2 &vector) const {
    return X * vector.X + Y * vector.Y;
}

float Vectors::Vector2::Cross(const Vectors::Vector2 &vector) const {
    return (X * vector.Y) - (Y * vector.X);
}
