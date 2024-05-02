#pragma once
#include "Vec2.h"

class CTransform {

	Vec2 pos;
	Vec2 speed;
	Vec2 scale;
	double angle;

public:
	CTransform();
};

class CShape {

public:
	CShape();
};

class CCollision {
	float radius;

public:
	CCollision();
};

class CInput {

public:
	CInput();
};

class CScore {
	int score;

public:
	CScore();
};

class CLifespan {
	int lifespan;

public:
	CLifespan();
};