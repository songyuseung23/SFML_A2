#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"

class Game {

	sf::RenderWindow m_window;
	EntityManager m_entities;
	//Entity m_player; // Game �����ڿ� Entity �⺻ Ŭ������ �����ϴ� ����.
	bool m_paused;
	bool m_running;

	void init();

public:

	Game(const std::string&);

	void run();
	void update();
	void sMovement();
	void sUserInput();
	void sRender();
	void sEnemySpawner();
	void sCollision();
};
