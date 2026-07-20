#pragma once
#include <iostream>
#include <string>
#include <memory>

enum class ShapeType
{
    Rectangle,
    Circle
};

class Shape
{
public:
    virtual void Draw() = 0;
    virtual ShapeType GetType() = 0;
    virtual std::shared_ptr<Shape> Clone() = 0;

    virtual ~Shape() = default;
};

class Rectangle : public Shape
{
private:
    int m_width{0};
    int m_height{0};

public:
    explicit Rectangle(int w, int h) : m_width{w}, m_height{h} {}
    Rectangle(const Rectangle& r) = default;

    void Draw() override
    {
        std::cout << "Rectangle drawing with width: " << m_width << " and height: " << m_height << "\n";
    }
    ShapeType GetType() override
    {
        return ShapeType::Rectangle;
    }
    std::shared_ptr<Shape> Clone() override
    {
        return std::make_shared<Rectangle>(*this);
    }
};

class Circle : public Shape
{
private:
    int m_radius{0};
public:
    explicit Circle(int r) : m_radius{r}{}
    Circle(const Circle& r) = default;

    void Draw() override
    {
        std::cout << "Circle drawing with radius: " << m_radius << "\n";
    }

    ShapeType GetType() override
    {
        return ShapeType::Circle;
    }

    std::shared_ptr<Shape> Clone() override
    {
        return std::make_shared<Circle>(*this);
    }
};



