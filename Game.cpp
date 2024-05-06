#include "Game.h"
#include <string>
#include <fstream>
#include <random>

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
		
		if (!m_paused) { // if game is running

			sEnemySpawner();
			sMovement();
			sCollision(); 
		}

		sUserInput(); 
		sRender();

		m_currentFrame++;
	}
}

void Game::sEnemySpawner() {
	
	// Spawns every specified frame in config file
	if (m_currentFrame % m_enemyConfig.SI == 0)
		spawnEnemy();
	
	// look through which enemy has dead
	for (auto e : m_entities.getEntities("enemy")) {
		// spawn small entities when enemy has dead.
		if (e->isActive() == false)
			spawnSmallEnemies(e);
	}
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

			Vec2 normal = e->cTransform->velocity.normalize();
			e->cTransform->velocity = normal * m_playerConfig.S;
		}

		auto transform = e->cTransform;
		e->cTransform->pos += e->cTransform->velocity;
		e->cShape->circle.setPosition(e->cTransform->pos.x , e->cTransform->pos.y);
	}
}

void Game::sCollision() {
	// TODO
	// enemy / window
	for (auto e : m_entities.getEntities("enemy"))
	{
		float CR = e->cCollision->radius;
		Vec2& currPos = e->cTransform->pos;
		Vec2& currVel = e->cTransform->velocity;

		if (currPos.x + CR >= m_window.getSize().x || currPos.x < CR)
			currVel.x *= -1.0f;
		if (currPos.y + CR >= m_window.getSize().y || currPos.y < CR)
			currVel.y *= -1.0f;
	}

	// enemy / bullet
	// enemy / player

	// player / enemy
	// player / window - prevent go further
	{
		float CR = m_player->cCollision->radius;
		Vec2& currPos = m_player->cTransform->pos;
		Vec2& currVel = m_player->cTransform->velocity;

		if (currPos.x + CR >= m_window.getSize().x)
			currPos.x = m_window.getSize().x - CR;
		else if (currPos.x < CR)
			currPos.x = CR;

		if (currPos.y + CR >= m_window.getSize().y)
			currPos.y = m_window.getSize().y - CR;
		else if (currPos.y < CR)
			currPos.y = CR;
	}

	// player / small enemies.

	// bullet enemy - mark either entities dead.
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

				case sf::Keyboard::Escape:
					m_running = false;
					break;

				case sf::Keyboard::P:
					{
						if (m_paused == true)
						{
							setPaused(false);
						}
						else 
						{
							setPaused(true);
						}
						break;
					}
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

			if (event.type == event.MouseButtonPressed) {

			}
		}
	}
}

void Game::sRender(){

	m_window.clear(sf::Color::Black);

	for (auto e : m_entities.getEntities()) {
		
		e->cShape->circle.rotate(1.0f);
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
	auto entity = m_entities.addEntity("enemy");

	float rx = randNumGenerator(m_enemyConfig.SR, m_window.getSize().x - m_enemyConfig.SR);
	float ry = randNumGenerator(m_enemyConfig.SR, m_window.getSize().y - m_enemyConfig.SR);

	// TODO : random generated velociy that magnitude is specified in config file with 'speed'
	float speed = randNumGenerator(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
	float dirx = randNumGenerator(0.0f, 1.0f);
	float diry = sqrt(1 - dirx * dirx);
	entity->cTransform = std::make_shared<CTransform>(
		Vec2(rx, ry), Vec2(dirx * speed, diry * speed), 0.0f
	);

	int rn = randNumGenerator(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
	int SR = randNumGenerator(0, 255);
	int SG = randNumGenerator(0, 255);
	int SB = randNumGenerator(0, 255);

	entity->cShape = std::make_shared<CShape>(
		m_enemyConfig.SR,
		rn,
		sf::Color(SR, SG, SB),
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
		m_enemyConfig.OT
	);

	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

	entity->cScore = std::make_shared<CScore>(rn * 100);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	// TODO : small enemies has lifespan
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}

int Game::randNumGenerator(int min, int max) {

	int range = max - min + 1;
	return min + rand() % range;
}

float Game::randNumGenerator(float min, float max) {

	std::random_device rd; 
	std::mt19937 gen(rd()); 

	std::uniform_real_distribution<float> dis(min, max); 

	return dis(gen);
}