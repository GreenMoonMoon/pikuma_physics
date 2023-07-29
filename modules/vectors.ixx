//
// Created by moonmoon on 29/07/23.
//

export module Vectors;
export namespace Vectors {
    struct Vector2 {
        float X;
        float Y;

        Vector2() : X(0), Y(0) {}

        Vector2(float X, float Y) : X(X), Y(Y) {}

        ~Vector2() = default;

        void Add(const Vector2 &vector);

        void Subtract(const Vector2 &vector);

        void Scale(const float scalar);

        void Multiply(const Vector2 &vector);

        void Divide(const Vector2 &vector);

        void Rotate(const float angle) const;

        float MagnitudeSquare() const;

        float Magnitude() const;

        Vector2 &Normalize();

        Vector2 Unit() const;

        Vector2 Normal() const;

        float Dot(const Vector2 &vector) const;

        float Cross(const Vector2 &vector) const;
    };
};
