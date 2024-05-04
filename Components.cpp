#include "Components.h"
#include <SFML/Graphics.hpp>

CTransform::CTransform(const Vec2& pos, const Vec2& speed, const float angle)
	: pos(pos), speed(speed), angle(angle) {}

CShape::CShape(float radius, int points, const sf::Color& fillColor, const sf::Color& outlineColor, int thickness) 
	: circle(radius, points)
{
	circle.setFillColor(fillColor);
	circle.setOutlineColor(outlineColor);
	circle.setOutlineThickness(thickness);
	circle.setOrigin(radius, radius);
}

CCollision::CCollision(float r) : radius(r) {}

CInput::CInput() { 

}

CScore::CScore(int s) : score(s) {}

CLifespan::CLifespan(int total) : remaining(total), total(total) {}