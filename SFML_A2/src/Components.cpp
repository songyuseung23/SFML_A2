#include "../header/Components.h"
#include <SFML/Graphics.hpp>

CTransform::CTransform(const Vec2& pos, const Vec2& velocity, const float angle)
	: pos(pos), velocity(velocity), angle(angle) {}

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