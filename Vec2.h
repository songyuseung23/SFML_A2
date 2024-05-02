#pragma once

class Vec2 {

public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float x, float y);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 operator + (const Vec2& rhs);
	Vec2 operator - (const Vec2& rhs);
	Vec2 operator * (float mul);
	Vec2 operator / (float div);

	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const float mul);
	void operator /= (const float div);

	const Vec2 normalize() const;
	double length() const;
	float dist(const Vec2& rhs) const;
};