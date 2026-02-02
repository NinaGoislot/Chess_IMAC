#include "Vector2D.hpp"
#include <cmath>

// Constructors
Vector2D::Vector2D()
    : x(0.0f), y(0.0f)
{
}

Vector2D::Vector2D(float x, float y)
    : x(x), y(y)
{
}

// Vector operations
Vector2D Vector2D::operator+(const Vector2D& other) const
{
    return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const
{
    return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::operator*(float scalar) const
{
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator/(float scalar) const
{
    if (scalar == 0.0f)
        return Vector2D(0.0f, 0.0f);

    return Vector2D(x / scalar, y / scalar);
}

// Utility functions
float Vector2D::length() const
{
    return std::sqrt(x * x + y * y);
}

float Vector2D::dotProduct(const Vector2D& other) const
{
    return x * other.x + y * other.y;
}

Vector2D Vector2D::normalized() const
{
    float len = length();

    if (len == 0.0f)
        return Vector2D(0.0f, 0.0f);

    return Vector2D(x / len, y / len);
}

// Comparison operators
bool Vector2D::operator==(const Vector2D& other) const
{
    return x == other.x && y == other.y;
}

bool Vector2D::operator!=(const Vector2D& other) const
{
    return !(*this == other);
}
