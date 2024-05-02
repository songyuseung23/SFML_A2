#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"

class Game {

	sf::RenderWindow m_window;
	EntityManager m_entities;
	//Entity m_player; // Game 생성자에 Entity 기본 클래스가 없습니다 에러.
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
