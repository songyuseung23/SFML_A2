#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Components.h"

class Entity {

	friend class EntityManager;

private:
	const std::string m_tag = "";
	const size_t m_id = 0;
	bool m_alive = true;

	Entity(const std::string&, const size_t);

public:

	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CScore> CScore;
	std::shared_ptr<CLifespan> cLifespan;

	const size_t id();
	const std::string& tag();
	void destroy();
	bool isActive();
};