#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Entity.h"

Entity::Entity(const std::string& tag, const size_t id) : m_tag(tag), m_id(id) {}

const size_t Entity::id() {
	return m_id;
}

const std::string& Entity::tag() {
	return m_tag;
}

void Entity::destroy() {
	m_alive = false;
}

bool Entity::isActive() {

	return m_alive;
}