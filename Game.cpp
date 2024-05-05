#include "Game.h"
#include <string>
#include <fstream>

// public
Game::Game(const std::string& config) {
	init(config);
}

#pragma region InitGame

// Read Config File, Instantiate EntityManager, Call spawnPlayer
void Game::init(const std::string& config) {

	std::ifstream file("Resources/" + config);

	if (!file.is_open())
		std::cerr << "Failed to Open File : " << config << '\n';

	std::string token;
	while (file >> token) {
		if (token == "Window") {
			int width, height, FL, FS;
			file >> width >> height >> FL >> FS;
			m_window.create(sf::VideoMode(width, height), "SFML_A2", FS == 1 ? sf::Style::Fullscreen : sf::Style::Default);
			m_window.setFramerateLimit(FL);
		}
		else if (token == "Font") {
			std::string fontPath;
			file >> fontPath;
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
	spawnPlayer();
}

void Game::spawnPlayer()
{
	auto entity = m_entities.addEntity("player");

	float mx = m_window.getSize().x / 2;
	float my = m_window.getSize().y / 2;

	// Attach Necessary Components to Entity.

	entity->cTransform = std::make_shared<CTransform>(
		Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f
	);

	entity->cShape = std::make_shared<CShape>(
		m_playerConfig.SR,
		m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		m_playerConfig.OT
	);

	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	entity->cInput = std::make_shared<CInput>();

	m_player = entity;
}

#pragma endregion

#pragma region Execute Each Frame

// Main Game Loop
void Game::run()
{
	while (m_running) {

		m_entities.Update();

		// if game is running
		if (!m_paused) {

			sEnemySpawner();
			sMovement();
			sCollision();
			sUserInput();
		}

		// execute whether pause or not
		sRender();
	}

	m_currentFrame++;
}

void Game::sEnemySpawner() {

}

void Game::sMovement() {

	for (auto e : m_entities.getEntities()) { 

		// only player has input component
		if (e->cInput) {
			e->cTransform->velocity = { 0,0 };

			if (e->cInput->up)
				e->cTransform->velocity.y = -1.0f;

			if (e->cInput->left)
				e->cTransform->velocity.x = -1.0f;

			if (e->cInput->down)
				e->cTransform->velocity.y = 1.0f;

			if (e->cInput->right)
				e->cTransform->velocity.x = 1.0f;
		}

		Vec2 normal = e->cTransform->velocity.normalize();
		e->cTransform->velocity = normal * m_playerConfig.S;

		std::cout << e->cTransform->velocity.x << ", " << e->cTransform->velocity.y << "\n";

		auto transform = e->cTransform;
		e->cTransform->pos += e->cTransform->velocity;
		e->cShape->circle.setPosition(e->cTransform->pos.x , e->cTransform->pos.y);
	}
}

void Game::sCollision() {

}

void Game::sUserInput() {

	sf::Event event;

	if (m_window.isOpen()) {
		while (m_window.pollEvent(event)) {
			if (event.type == event.Closed) {
				m_window.close();
				m_running = false;
			}

			if (event.type == event.KeyPressed) {

				switch (event.key.code) {

				case sf::Keyboard::W:
					m_player->cInput->up = true;
					break;

				case sf::Keyboard::A:
					m_player->cInput->left = true;
					break;

				case sf::Keyboard::S:
					m_player->cInput->down = true;
					break;

				case sf::Keyboard::D:
					m_player->cInput->right = true;
					break;
				}
			}

			if (event.type == event.KeyReleased) {

				switch (event.key.code) {

				case sf::Keyboard::W:
					m_player->cInput->up = false;
					break;

				case sf::Keyboard::A:
					m_player->cInput->left = false;
					break;

				case sf::Keyboard::S:
					m_player->cInput->down = false;
					break;

				case sf::Keyboard::D:
					m_player->cInput->right = false;
					break;
				}
			}
		}
	}
}

void Game::sRender(){

	m_window.clear(sf::Color::Black);
	for (auto e : m_entities.getEntities()) {

		m_window.draw(e->cShape->circle);
	}
	
	m_window.display();
}

#pragma endregion

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::sLifespan()
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



