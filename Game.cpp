#include "Game.h"
#include <string>
#include <fstream>

void Game::init(const std::string& config) {

	// TODO : read config file.
	std::ifstream file("Resources/" + config);

	if (!file.is_open())
		std::cerr << "Failed to Open File : " << config << '\n';
	
	std::string token;
	file >> token;

	if (token == "Window") {
		int width, height, FL, FS;
		file >> width >> height >> FL >> FS;
		m_window.create(sf::VideoMode(width, height), "SFML_A2", FS == 1 ? sf::Style::Fullscreen : sf::Style::Default);
		m_window.setFramerateLimit(FL);
	}
	else if (token == "Font") {
		std::string fontPath;
		if (m_font.loadFromFile("Resources/" + fontPath)) {
			int size, R, G, B;
			file >> size >> R >> G >> B;
			m_text.setFillColor(sf::Color(R, G, B));
			m_text.setCharacterSize(size);
		}
		else
			std::cout << "Failed to load Font : " << fontPath << '\n';
	}
	else if (token == "Player") {
		file >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
	}
	else if (token == "Enemy") {
		file >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
	}
	else if (token == "Bullet")
	{
		file >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
	}
}

void Game::setPaused(bool paused)
{

}

void Game::sMovement() {

}

void Game::sUserInput(){

}

void Game::sLifespan()
{
}

void Game::sRender(){

}

void Game::sEnemySpawner(){

}

void Game::sCollision(){

}

void Game::spawnPlayer()
{
}

void Game::spawnEnemy()
{
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}

Game::Game(const std::string& config) {
	init(config);
}

void Game::run()
{
	// test 
	while (m_window.isOpen()) {

		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window.close();
		}

	}
}

