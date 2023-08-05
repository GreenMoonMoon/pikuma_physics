//
// Created by moonmoon on 29/07/23.
//
#include <cmath>

namespace Vectors {
    struct Vector2 {
        float X;
        float Y;

        Vector2();
        Vector2(float X, float Y);
        Vector2(float v);

        ~Vector2() = default;

        void Add(const Vector2 &vector);
        void Subtract(const Vector2 &vector);
        void Scale(float scalar);
        Vector2 Rotate(float angle) const;
        float MagnitudeSquare() const;
        float Magnitude() const;
        Vector2 &Normalize();
        Vector2 Unit() const;
        Vector2 Normal() const;
        float Dot(const Vector2 &vector) const;
        float Cross(const Vector2 &vector) const;
    };
};