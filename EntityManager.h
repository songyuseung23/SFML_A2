#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVector;
typedef std::map<std::string, EntityVector> EntityMap;

class EntityManager {

	EntityVector m_entites;
	EntityVector m_toAdd;
	EntityMap m_entityMap;
	size_t m_totalEntites = 0;

	void init();

public:
	EntityManager();
	void Update();
	std::shared_ptr<Entity> addEntity(const std::string&);
	EntityVector& getEntities();
	EntityVector& getEntities(const std::string&);
};