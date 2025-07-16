#pragma once
#include "Framework/BaseLevel.h"
#include "TextureManager.h"
#include "Player.h"
#include "Lives.h"
#include "Seagull.h"
#include <random>
#include <iostream>

class RunnerLevel : BaseLevel
{
public:	
	RunnerLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, TextureManager* tm);
	~RunnerLevel();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;
	bool colliding(GameObject obj);
	void reset();

private:
	TextureManager* textMan;
	Player p;
	Lives lives;
	Seagull seagull;
	std::vector<GameObject> BGs;
	std::vector<GameObject> jumpables;
	std::vector<GameObject> kickables;
	std::vector<GameObject> explosions;
	std::vector<GameObject> platforms;
	std::vector<float> explosionTimer;
	sf::Font timerFont;
	sf::Text timerText;


	GameObject moon;
	GameObject finishLine;

	float distance;
	float travelled = 0.f;
	float speed;

	float seagullSpawnTimer = 0.f;
	bool seagullSpawned = false;

	const float MAX_SPEED = 650;
	const float ACCELERATION = 250;

	int hits = 0;
	float time = 0.f;
	float objects = 0.f;

	void spawnObstacle(float minX);

};

