#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>

class CTransform {

	Vec2 pos = { 0, 0 };
	Vec2 speed = { 0,0 };
	float angle = 0;

public:
	CTransform(const Vec2& pos, const Vec2& speed, const float angle);
};

class CShape {

public:
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& fillColor, const sf::Color& outlineColor, int thickness);
};

class CCollision {

public:
	float radius = 0;
	CCollision(float r);
};

class CInput {

public:
	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
	bool shoot = false;

	CInput();
};

class CScore {

public:
	int score = 0;
	CScore(int s);
};

class CLifespan {

public:
	int remaining = 0;
	int total = 0;

	CLifespan(int total);
};