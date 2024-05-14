#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../header/Entity.h"
#include "../header/EntityManager.h"

void EntityManager::init() {
	m_entities = std::vector<std::shared_ptr<Entity>>();
	m_entityMap = std::map<std::string, EntityVector>();
}

EntityManager::EntityManager() {
	init();
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {

	auto e = std::make_shared<Entity>(*( new Entity(tag, m_totalEntites++) )); 
	m_toAdd.push_back(e);
	return e;
}

void EntityManager::Update() {

	for (auto e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_toAdd.clear();

	for (auto e : m_entities) {
		if (e == NULL) // error prevent : iterator error
			continue;

		// if entity is dead
		if (e->isActive() == false)
		{	
			// erase pointer from m_entities
			m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), e), m_entities.end()); 

			// erase pointer from map that holds entity vector by tag
			EntityVector& temp = m_entityMap[e->tag()];
			temp.erase(std::remove(temp.begin(), temp.end(), e), temp.end());

			// erase object that pointed by e
			e.reset();
		}
	}
}

EntityVector& EntityManager::getEntities() {
	return m_entities;
}

EntityVector& EntityManager::getEntities(const std::string& tag) {
	return m_entityMap[tag];
}