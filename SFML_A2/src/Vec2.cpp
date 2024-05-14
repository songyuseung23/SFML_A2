
#include "../header/Vec2.h"
#include <math.h>

Vec2::Vec2() {}

Vec2::Vec2(float x, float y) : x(x), y(y) {}

Vec2 Vec2::operator + (const Vec2& rhs) const {
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (float mul) const {
	return Vec2(x * mul, y * mul);
}

Vec2 Vec2::operator / (float div) const {
	return Vec2(x / div, y / div);
}

void Vec2::operator += (const Vec2& rhs) {
	x += rhs.x;
	y += rhs.y;
};

void Vec2::operator -= (const Vec2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
};

void Vec2::operator *= (const float mul) {
	x *= mul;
	y *= mul;
};

void Vec2::operator /= (const float div) {
	x /= div;
	y /= div;
};

bool Vec2::operator == (const Vec2& rhs) const {

	return x == rhs.x && y == rhs.y;
};

bool Vec2::operator != (const Vec2& rhs) const {

	return x != rhs.x || y != rhs.y;
};

const Vec2 Vec2::normalize() const {

	if (this->length() == 0)
		return Vec2(0, 0);

	return 
		Vec2(x / this->length(), y / this->length());
}

double Vec2::length() const{

	return sqrtf(x * x + y * y);
};

float Vec2::dist(const Vec2& rhs) const
{
	return sqrtf((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
}
;