#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Entity.h"
#include "EntityManager.h"

void EntityManager::init() {
	m_entites = std::vector<std::shared_ptr<Entity>>();
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
		m_entites.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_toAdd.clear();

	for (auto e : m_entites) {
		// remove e which marked as dead from m_entities
		// remove e which marked as dead from m_entityMap

	}
}

EntityVector& EntityManager::getEntities() {
	return m_entites;
}

EntityVector& EntityManager::getEntities(const std::string& tag) {
	return m_entityMap[tag];
}