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
				m_text.setFont(m_font);
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
		sLifespan();
	}
}

void Game::sEnemySpawner() {
	
	// Spawns every specified frame in config file
	if (m_currentFrame % m_enemyConfig.SI == 0)
		spawnEnemy();
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

		if (e->tag() == "shield")
			e->cTransform->pos = m_player->cTransform->pos; // always follow player's position

		auto transform = e->cTransform;
		e->cTransform->pos += e->cTransform->velocity;
		e->cShape->circle.setPosition(e->cTransform->pos.x , e->cTransform->pos.y);
	}
}

void Game::sCollision() {

	// increase shiled's Collision Radius gradually
	for (auto shield : m_entities.getEntities("shield")) {

		float lifespan = shield->cLifespan->total;
		float& CR = shield->cCollision->radius;
		CR += 3 * m_playerConfig.CR / lifespan;
	}
		
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

	// enemy / bullet, shield
	for (auto e : m_entities.getEntities("enemy"))
	{
		for (auto b : m_entities.getEntities("bullet"))
		{
			Vec2& ePos = e->cTransform->pos;
			Vec2& bPos = b->cTransform->pos;

			double dis = (ePos - bPos).length();

			// when two entities collide
			if (dis <= e->cCollision->radius + b->cCollision->radius)
			{
				m_score += e->cScore->score;
				e->destroy();
				spawnSmallEnemies(e); 
				b->destroy();
			}
		}

		for (auto shield : m_entities.getEntities("shield"))
		{
			Vec2& ePos = e->cTransform->pos;
			Vec2& shieldPos = shield->cTransform->pos;

			double dis = (ePos - shieldPos).length();

			if (dis <= e->cCollision->radius + shield->cCollision->radius)
			{
				m_score += e->cScore->score;
				e->destroy();
			}
		}
	}

	// player / enemy
	for (auto e : m_entities.getEntities("enemy"))
	{
		Vec2& ePos = e->cTransform->pos;
		Vec2& playerPos = m_player->cTransform->pos;

		double dis = (ePos - playerPos).length();

		// when two entities collide
		if (dis <= e->cCollision->radius + m_player->cCollision->radius)
		{
			e->destroy();
			
			float mx = m_window.getSize().x / 2;
			float my = m_window.getSize().y / 2;
			
			m_score -= e->cScore->score;
			m_player->cTransform->pos.x = mx;
			m_player->cTransform->pos.y = my;
		}
	}
	
	// player / small enemies.
	for (auto e : m_entities.getEntities("small_enemy"))
	{
		Vec2& ePos = e->cTransform->pos;
		Vec2& playerPos = m_player->cTransform->pos;

		double dis = (ePos - playerPos).length();

		// when two entities collide
		if (dis <= e->cCollision->radius + m_player->cCollision->radius)
		{
			e->destroy();

			float mx = m_window.getSize().x / 2;
			float my = m_window.getSize().y / 2;

			m_score -= e->cScore->score;
			m_player->cTransform->pos.x = mx;
			m_player->cTransform->pos.y = my;
		}

		for (auto shield : m_entities.getEntities("shield"))
		{
			Vec2& ePos = e->cTransform->pos;
			Vec2& shieldPos = shield->cTransform->pos;

			double dis = (ePos - shieldPos).length();

			if (dis <= e->cCollision->radius + shield->cCollision->radius)
				e->destroy();
		}
	}
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
							setPaused(false);
						else 
							setPaused(true);
						break;
					}
				case sf::Keyboard::R:
					{
						if (m_cooltime <= 0)
						{
							m_cooltime = spawnSpecialWeapon(m_player);
							// Show message in window.
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
				if (event.mouseButton.button == sf::Mouse::Left) 
				{
					Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
					spawnBullet(m_player, mousePos);
				}

				if (event.mouseButton.button == sf::Mouse::Right)
				{
					spawnSpecialWeapon(m_player);
				}
			}
		}
	}
}

void Game::sRender(){

	m_window.clear(sf::Color::Black);

	for (auto e : m_entities.getEntities()) {
		
		e->cShape->circle.rotate(1.0f);
		// entity only which has cLifespan Component : bullet, small enemies, shield
		if (e->cLifespan) 
		{
			int lifespan = e->cLifespan->total;
			sf::Color FC = e->cShape->circle.getFillColor();
			if (FC.a <= 0)
				FC.a = 0;
			else
				FC.a -= (255 / lifespan);
			e->cShape->circle.setFillColor(FC);

			sf::Color OC = e->cShape->circle.getOutlineColor();
			if (OC.a <= 0)
				OC.a = 0;
			else
				OC.a -= (255 / lifespan); 
			e->cShape->circle.setOutlineColor(OC);
		}

		// scale shield scale gradually
		if (e->tag() == "shield")
		{
			float lifespan = e->cLifespan->total;
			float scaleF = e->cShape->circle.getScale().x + 3 / lifespan;
			e->cShape->circle.setScale(sf::Vector2f(scaleF, scaleF));
		}
		m_window.draw(e->cShape->circle);
	}
	
	{
		m_text.setString("Score : " + std::to_string( m_score ));
		m_window.draw(m_text);
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
	for (auto e : m_entities.getEntities()) {
		if (!e->cLifespan)
			continue;

		// entity that has cLifespan component : bullet, shield, small enemy
		e->cLifespan->remaining -= 1;
		if (e->cLifespan->remaining == 0)
			e->destroy();
	}
	if(m_cooltime > 0)
		m_cooltime--;
}

void Game::spawnEnemy()
{
	auto entity = m_entities.addEntity("enemy");

	float rx = randNumGenerator(m_enemyConfig.SR, m_window.getSize().x - m_enemyConfig.SR);
	float ry = randNumGenerator(m_enemyConfig.SR, m_window.getSize().y - m_enemyConfig.SR);

	// Randomize init pos and velociy
	float speed = randNumGenerator(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
	float dirx = randNumGenerator(-1.0f, 1.0f);
	int sign = randNumGenerator(0, 1); 
	int arr[] = {-1, 1};
	float diry = sqrt(1 - dirx * dirx) * arr[sign];

	entity->cTransform = std::make_shared<CTransform>(
		Vec2(rx, ry), Vec2(dirx * speed, diry * speed), 0.0f
	);

	// Randomize init vertices and color
	int V = randNumGenerator(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
	int SR = randNumGenerator(0, 255);
	int SG = randNumGenerator(0, 255);
	int SB = randNumGenerator(0, 255);

	entity->cShape = std::make_shared<CShape>(
		m_enemyConfig.SR,
		V,
		sf::Color(SR, SG, SB),
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
		m_enemyConfig.OT
	);

	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

	entity->cScore = std::make_shared<CScore>(V * 100);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	float originCR = m_enemyConfig.CR;
	float originSR = m_enemyConfig.SR;
	float originS = entity->cTransform->velocity.length();
	Vec2 originPos = entity->cTransform->pos;
	int originV = entity->cShape->circle.getPointCount();

	for(int i = 0; i < originV ;i++)
	{
		auto se = m_entities.addEntity("small_enemy");

		se->cCollision = std::make_shared<CCollision>(originCR / 2);

		float rad = ((360 / originV) * i) * 3.14 / 180;
		Vec2 vel = { originS * cosf(rad), originS * sinf(rad) };
		se->cTransform = std::make_shared<CTransform>(originPos, vel, 0.0f);

		se->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);

		se->cShape = std::make_shared<CShape>(*(entity->cShape));
		se->cShape->circle.setRadius(originSR / 2);

		se->cScore = std::make_shared<CScore>(entity->cScore->score);
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> player, Vec2& target)
{
	auto entity = m_entities.addEntity("bullet");

	Vec2 dir = target - player->cTransform->pos;
	Vec2 vel = dir.normalize() * m_bulletConfig.S;

	entity->cTransform = std::make_shared<CTransform>(player->cTransform->pos, vel, 0.0f);

	entity->cShape = std::make_shared<CShape>(
		m_bulletConfig.SR,
		m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
		m_bulletConfig.OT
	);

	entity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	
	entity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

int Game::spawnSpecialWeapon(std::shared_ptr<Entity> player)
{
	// TODO : Radial shield that scales as time goes, and also has lifespan that 
	auto shield = m_entities.addEntity("shield");

	shield->cTransform = std::make_shared<CTransform>(player->cTransform->pos, Vec2(0, 0), 0.0f);
	
	shield->cShape = std::make_shared<CShape>(
		m_playerConfig.SR,
		m_playerConfig.V,
		sf::Color::Transparent,
		sf::Color::Magenta,
		m_playerConfig.OT
	);

	const int lifespan = 120; // 60fr * 2s = 180frame
	const int cooltime = 600; // 10s
	shield->cLifespan = std::make_shared<CLifespan>(lifespan); // change

	shield->cCollision = std::make_shared<CCollision>(m_playerConfig.CR); // changes as frame pass

	return cooltime;
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