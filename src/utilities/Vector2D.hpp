#pragma once

// Represents a lightweight 2D vector used for board coordinates and offsets.
class Vector2D {
public:
    // Constructors
    Vector2D();
    Vector2D(float x, float y);

    // Vector operations
    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(float scalar) const;
    Vector2D operator/(float scalar) const;

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }

private:
    // Internal x coordinate value.
    float x;
    // Internal y coordinate value.
    float y;

    // Utility functions
    float    length() const;
    float    dotProduct(const Vector2D& other) const;
    Vector2D normalized() const;

    // Comparison operators
    bool operator==(const Vector2D& other) const;
    bool operator!=(const Vector2D& other) const;
};
