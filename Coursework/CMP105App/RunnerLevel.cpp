#include "RunnerLevel.h"
#include "Framework/Collision.h"

int getRandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(gen);
}

RunnerLevel::RunnerLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, TextureManager* tm)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;
	textMan = tm;
	std::srand(static_cast<unsigned>(std::time(nullptr)));


	//Set up Timer
	if (!timerFont.loadFromFile("font/montS.ttf")) {
		std::cerr << "Failed to load font for timer!" << std::endl;
	}
	timerText.setFont(timerFont);
	timerText.setCharacterSize(32);
	timerText.setFillColor(sf::Color::Black);
	timerText.setPosition(10.f, 10.f);
	timerText.setString("Time: 0.0s");


	// setup BGs as ten images next to each other. base dimensions 1024x1024
	float bgScalar = hwnd->getSize().y / 1024.0f;
	for (int i = 0; i < 21; ++i)
	{
		GameObject bg;
		bg.setTexture(&textMan->getTexture("bg_Scroll"));
		bg.setSize(sf::Vector2f(1024.0f*bgScalar , 1024.0f*bgScalar));
		bg.setPosition(i * 1024 * bgScalar, 0);
		BGs.push_back(bg);
	}


	

	
	// setup Player
	p.setPosition(window->getSize().x * 0.5, window->getSize().y * 0.6);
	speed = 0.f;
	p.setInput(in);
	p.setGroundLevel(window->getSize().y * 0.6 + p.getSize().y);
	p.activateRunningMechanics(true);
	float leftBound = 0.f;
	float rightBound = static_cast<float>(window->getSize().x);
	p.setLevelEdges(leftBound, rightBound);
	p.setCollisionBox(0, 0, p.getSize().x, p.getSize().y);


	//spawn seagull
	float seagullY = window->getSize().y * 0.05f;
	float seagullX = window->getSize().x * 1.2f; // 20% off-screen right
	seagull.setPosition(seagullX, seagullY);
	float seagullSpeed = 300.0f; // adjust speed as needed
	seagull.setVelocity(-seagullSpeed, 0.0f);


	//set up Lives
	lives.setPosition(sf::Vector2f(30, window->getSize().y - 70));


	p.setDamaged(0.5);

}

RunnerLevel::~RunnerLevel()
{
}

void RunnerLevel::handleInput(float dt)
{
	
	if (input->isKeyDown(sf::Keyboard::Enter) && !p.isKicking())
	{
		p.setKicking(0.5);
	}
	p.handleInput(dt);
}

bool RunnerLevel::colliding(GameObject obj)
{
	if (obj.getGlobalBounds().intersects(p.getGlobalBounds()))
	{
		return true;
	}

	return false;
}

void RunnerLevel::update(float dt)
{

	time += dt;

	//Seagull Spawn Timer
	seagullSpawnTimer += dt;
	if (!seagullSpawned && seagullSpawnTimer >= 30.f) {
		seagullSpawned = true;
	}

	// Set the default ground level (base ground)
	float defaultGroundLevel = window->getSize().y * 0.6f + p.getSize().y;
	bool onPlatform = false;
	float platformGroundLevel = defaultGroundLevel;


	// Check all platforms for collision
	for (auto& platform : platforms)
	{
		if (Collision::checkBoundingBox(&p, &platform))
		{
			auto playerBounds = p.getGlobalBounds();
			auto platformBounds = platform.getGlobalBounds();

			float playerFeet = playerBounds.top + playerBounds.height;
			float platformTop = platformBounds.top;
			float playerLeft = playerBounds.left;
			float playerRight = playerBounds.left + playerBounds.width;
			float platformLeft = platformBounds.left;
			float platformRight = platformBounds.left + platformBounds.width;

			// Only land if player's feet are above the platform and close enough to land
			if(
				p.getVelocity().y >= 0.0f &&
				playerFeet > platformTop && playerFeet < platformTop + 20.0f &&
				playerRight > platformLeft && playerLeft < platformRight
			)
			{
				platformGroundLevel = platformTop;
				onPlatform = true;
				break; 
			}
		}
	}

	// Set the ground level based on collision
	if (onPlatform)
	{
		p.setGroundLevel(platformGroundLevel);
		std::cout << "Player landed on platform at y=" << platformGroundLevel << std::endl;
	}
	else
	{
		p.setGroundLevel(defaultGroundLevel);
	}


	if (seagullSpawned) {
		seagull.update(dt, window->getSize().x);
	}
	p.update(dt);
	lives.update(dt);

	

	// scroll stuff
	for (GameObject& bg : BGs)
	{
		bg.move(-dt * speed, 0);
		if (bg.getPosition().x + bg.getSize().x < 0) {
			// Move to the rightmost position
			float maxX = 0.f;
			for (const auto& other : BGs)
				maxX = std::max(maxX, other.getPosition().x);
			bg.setPosition(maxX + bg.getSize().x, 0);
		}
	}
	for (GameObject& j : jumpables)
	{
		j.move(-dt * speed, 0);
	}
	for (GameObject& k : kickables)
	{
		k.move(-dt * speed, 0);
	}
	for (GameObject& p2 : platforms)
	{
		p2.move(-dt * speed, 0);
	}
	for (GameObject& e : explosions)
	{
		e.move(-dt * speed, 0);
	}
	//Move seagull egg when it is dropping
	if (seagull.egg.isAlive() && seagull.eggReadyToDrop) {
		seagull.egg.move(-dt * speed, 0);
	}

	// Remove jumpables off screen
	jumpables.erase(
		std::remove_if(jumpables.begin(), jumpables.end(),
			[this](const GameObject& obj) {
				return obj.getPosition().x + obj.getSize().x < 0;
			}),
		jumpables.end()
	);

	// Remove kickables off screen
	kickables.erase(
		std::remove_if(kickables.begin(), kickables.end(),
			[this](const GameObject& obj) {
				return obj.getPosition().x + obj.getSize().x < 0;
			}),
		kickables.end()
	);

	// Find the rightmost obstacle position
	float rightmost = window->getSize().x;
	for (const auto& obj : jumpables)
		rightmost = std::max(rightmost, obj.getPosition().x + obj.getSize().x);
	for (const auto& obj : kickables)
		rightmost = std::max(rightmost, obj.getPosition().x + obj.getSize().x);
	for (const auto& obj : platforms)
		rightmost = std::max(rightmost, obj.getPosition().x + obj.getSize().x);

	// Always keep at least N obstacles ahead of the player
	while (rightmost < window->getSize().x * 2) {
		float gap = getRandomInt(300, 600); // random gap between obstacles
		rightmost += gap;
		RunnerLevel::spawnObstacle(rightmost);
	}

	// speed increases to max. Changes animation speed.
	if (speed < MAX_SPEED)
		speed += dt * ACCELERATION;
	if (!p.isKicking())
		p.currentAnimation->setFrameSpeed(25 / speed);

	travelled += speed * dt;

	// explosions
	for (int i = 0; i < explosions.size(); ++i)
	{
		explosionTimer[i] += dt;
		if (explosionTimer[i] > 0.05 && explosionTimer[i] < 0.1)
		{
			sf::Texture* tex = &textMan->getTexture("explosion2");
			explosions[i].setTexture(tex);
			sf::Vector2u texSize = tex->getSize();
			explosions[i].setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
		}
		if (explosionTimer[i] > 0.1 && explosionTimer[i] < 0.15)
		{
			sf::Texture* tex = &textMan->getTexture("explosion3");
			explosions[i].setTexture(tex);
			sf::Vector2u texSize = tex->getSize();
			explosions[i].setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
		}
		if (explosionTimer[i] > 0.15 && explosionTimer[i] < 0.2)
		{
			sf::Texture* tex = &textMan->getTexture("explosion4");
			explosions[i].setTexture(tex);
			sf::Vector2u texSize = tex->getSize();
			explosions[i].setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
		}
		if (explosionTimer[i] > 0.2 && explosionTimer[i] < .25)
		{
			sf::Texture* tex = &textMan->getTexture("explosion5");
			explosions[i].setTexture(tex);
			sf::Vector2u texSize = tex->getSize();
			explosions[i].setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
		}
		explosions[i].setSize(sf::Vector2f(window->getSize().y * 0.1, window->getSize().y * 0.1));
		if (explosionTimer[i] > 0.25)
		{
			explosions.erase(explosions.begin() + i);
			explosionTimer.erase(explosionTimer.begin() + i);
		}
	}

	// check for collisions
	if (p.isDamaged())
	{
		// if you're damaged you can keep going.
		return;
	}
	for (int i = 0; i < kickables.size(); ++i)// k : kickables)
	{
		GameObject k = kickables[i];
		if (colliding(k))
		{
			if (p.isKicking())
			{
				// explode the box
				kickables.erase(kickables.begin() + i);
				audio->playSoundbyName("success");
				GameObject explosion;
				explosion.setSize(sf::Vector2f(window->getSize().y * 0.1, window->getSize().y * 0.1));
				explosion.setPosition(p.getPosition());
				explosion.setTexture(&textMan->getTexture("explosion1"));
				explosions.push_back(explosion);
				explosionTimer.push_back(0.f);
			}
			else
			{
				//take damage
				hits++;
				lives.takeDamage();
				speed = 0.f;
				kickables.erase(kickables.begin() + i);
				audio->playSoundbyName("death");

				//end game if out of lives
				if (lives.isDead()) {
					gameState->addResult("l2deaths", hits);
					gameState->addResult("l2time", time);
					gameState->addResult("l2distance", travelled/100);
					if (gameState->getSingleRun()) {
						gameState->setCurrentState(State::ENDGAME);
					}
					else {
						gameState->setCurrentState(State::PRE_THREE);
					}
					return;
				}
			}
		}
	}
	// check for collisions
	for (int i = 0; i < jumpables.size(); ++i)
	{
		GameObject j = jumpables[i];
		if (colliding(j))
		{
			//take damage
			lives.takeDamage();
			speed = 0.f;
			jumpables.erase(jumpables.begin() + i);
			audio->playSoundbyName("death");

			//end game if out of lives
			if (lives.isDead()) {
				gameState->addResult("l2deaths", hits);
				gameState->addResult("l2time", time);
				gameState->addResult("l2distance", travelled / 100);
				if (gameState->getSingleRun()) {
					gameState->setCurrentState(State::ENDGAME);
				}
				else {
					gameState->setCurrentState(State::PRE_THREE);
				}
				return;
			}
		}
	}

	// Egg collision detection
	if (seagull.egg.isAlive()) {
		// Check collision with platforms
		for (const auto& platform : platforms) {
			if (seagull.egg.getGlobalBounds().intersects(platform.getGlobalBounds())) {
				seagull.egg.setAlive(false);
				seagull.carryingEgg = false;

				// Spawn explosion at egg position
				GameObject explosion;
				explosion.setSize(sf::Vector2f(window->getSize().y * 0.1f, window->getSize().y * 0.1f));
				explosion.setPosition(seagull.egg.getPosition());
				sf::Texture* tex = &textMan->getTexture("explosion1");
				explosion.setTexture(tex);
				sf::Vector2u texSize = tex->getSize();
				explosion.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
				explosions.push_back(explosion);
				explosionTimer.push_back(0.f);

				break;
			}
		}

		// Check collision with player
		if (seagull.egg.getGlobalBounds().intersects(p.getGlobalBounds())) {
			seagull.egg.setAlive(false);
			seagull.carryingEgg = false;

			// Spawn explosion at egg position
			GameObject explosion;
			explosion.setSize(sf::Vector2f(window->getSize().y * 0.1f, window->getSize().y * 0.1f));
			explosion.setPosition(seagull.egg.getPosition());
			sf::Texture* tex = &textMan->getTexture("explosion1");
			explosion.setTexture(tex);
			sf::Vector2u texSize = tex->getSize();
			explosion.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
			explosions.push_back(explosion);
			explosionTimer.push_back(0.f);
			//take damage
			lives.takeDamage();
			speed = 0.f;
			audio->playSoundbyName("death");

			//end game if out of lives
			if (lives.isDead()) {
				gameState->addResult("l2deaths", hits);
				gameState->addResult("l2time", time);
				gameState->addResult("l2distance", travelled / 100);
				if (gameState->getSingleRun()) {
					gameState->setCurrentState(State::ENDGAME);
				}
				else {
					gameState->setCurrentState(State::PRE_THREE);
				}
				return;
			}
		}

		// Check collision with ground
		float groundY = window->getSize().y * 0.6f + p.getSize().y; // same as player ground
		if (seagull.egg.getPosition().y + seagull.egg.getSize().y >= groundY) {
			seagull.egg.setAlive(false);
			seagull.carryingEgg = false;

			// Spawn explosion at egg position
			GameObject explosion;
			explosion.setSize(sf::Vector2f(window->getSize().y * 0.1f, window->getSize().y * 0.1f));
			explosion.setPosition(seagull.egg.getPosition());
			sf::Texture* tex = &textMan->getTexture("explosion1");
			explosion.setTexture(tex);
			sf::Vector2u texSize = tex->getSize();
			explosion.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
			explosions.push_back(explosion);
			explosionTimer.push_back(0.f);
		}
	}

	timerText.setString("Time: " + std::to_string(static_cast<int>(time * 10) / 10.0f) + "s");

}

void RunnerLevel::render()
{
	beginDraw();
	for(GameObject bg : BGs) window->draw(bg);
	for (GameObject j : jumpables) window->draw(j);
	for (GameObject k : kickables) window->draw(k);
	for (GameObject p2 : platforms) window->draw(p2);
	window->draw(p);
	if (seagullSpawned) {
		window->draw(seagull);
	}
	if (seagull.egg.isAlive())
		window->draw(seagull.egg);
	for (GameObject e : explosions) window->draw(e);
	window->draw(moon);
	window->draw(lives);
	window->draw(timerText);
	endDraw();
}

void RunnerLevel::reset()
{
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	hits = 0;
	time = 0.f;
	objects = 0.f;
	travelled = 0.f;
	lives.resetLives();

	// reset seagull
	seagullSpawned = false;
	seagullSpawnTimer = 0.f;

	// setup Player
	p.setPosition(30, window->getSize().y * 0.6);
	speed = 0.f;

	BGs.clear();
	kickables.clear();
	jumpables.clear();

	// setup BGs as ten images next to each other. base dimensions 1024x1024
	float bgScalar = window->getSize().y / 1024.0f;
	for (int i = 0; i < 21; ++i)
	{
		GameObject bg;
		bg.setTexture(&textMan->getTexture("bg_Scroll"));
		bg.setSize(sf::Vector2f(1024.0f * bgScalar, 1024.0f * bgScalar));
		bg.setPosition(i * 1024 * bgScalar, 0);
		BGs.push_back(bg);
	}

	
	finishLine.setPosition(distance, window->getSize().y * 0.5);
	p.setDamaged(0.5);
}

void RunnerLevel::spawnObstacle(float minX)
{
	GameObject newObj;
	float yJump = window->getSize().y * 0.6f;
	float yPlatform = window->getSize().y * 0.4f;
	float yKick = window->getSize().y * 0.5f;
	float size = window->getSize().y * 0.1f;
	int seed = getRandomInt(0, 5); // Now 0-5 for 6 cases

	switch (seed)
	{
	case 0: // kickable
		newObj.setTexture(&textMan->getTexture("kickable"));
		newObj.setPosition(minX, yKick);
		newObj.setSize(sf::Vector2f(size, size * 2));
		kickables.push_back(newObj);
		break;
	case 1: // jumpable
		newObj.setTexture(&textMan->getTexture("jumpable"));
		newObj.setPosition(minX, yJump);
		newObj.setSize(sf::Vector2f(size, size));
		jumpables.push_back(newObj);
		break;
	case 2: // Platform
		newObj.setTexture(&textMan->getTexture("Platform"));
		newObj.setPosition(minX, yPlatform);
		newObj.setSize(sf::Vector2f(size * 4.5, size));
		newObj.setCollisionBox(0, 0, newObj.getSize().x, newObj.getSize().y);
		platforms.push_back(newObj);
		break;
	case 3: // two jumpables
		newObj.setTexture(&textMan->getTexture("jumpable"));
		newObj.setPosition(minX, yJump);
		newObj.setSize(sf::Vector2f(size, size));
		jumpables.push_back(newObj);
		{
			GameObject secondObj = newObj;
			secondObj.setPosition(minX + size, yJump);
			jumpables.push_back(secondObj);
		}
		break;
	case 4: // platform and kickable
	{
		// Platform
		GameObject platformObj;
		platformObj.setTexture(&textMan->getTexture("Platform"));
		platformObj.setPosition(minX, yPlatform);
		platformObj.setSize(sf::Vector2f(size * 4.5, size));
		platformObj.setCollisionBox(0, 0, platformObj.getSize().x, platformObj.getSize().y);
		platforms.push_back(platformObj);

		// Kickable
		GameObject kickableObj;
		kickableObj.setTexture(&textMan->getTexture("kickable"));
		kickableObj.setPosition(minX, yKick);
		kickableObj.setSize(sf::Vector2f(size, size * 2));
		kickables.push_back(kickableObj);
	}
	break;
	case 5: // platform and jumpable
	{
		// Platform
		GameObject platformObj;
		platformObj.setTexture(&textMan->getTexture("Platform"));
		platformObj.setPosition(minX, yPlatform);
		platformObj.setSize(sf::Vector2f(size * 4.5, size));
		platformObj.setCollisionBox(0, 0, platformObj.getSize().x, platformObj.getSize().y);
		platforms.push_back(platformObj);

		// Jumpable
		GameObject jumpableObj;
		jumpableObj.setTexture(&textMan->getTexture("jumpable"));
		jumpableObj.setPosition(minX, yJump);
		jumpableObj.setSize(sf::Vector2f(size, size));
		jumpables.push_back(jumpableObj);
	}
	break;
	}
}