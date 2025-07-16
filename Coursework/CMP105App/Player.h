#pragma once

#include "Framework/GameObject.h"
#include "Framework/Animation.h"

class Player : public GameObject
{
public:
	Player();
	~Player();

	const bool isOnGround();
	void handleInput(float dt);
	void setDamaged(float timer);
	bool isDamaged();
	void update(float dt);
	void setFlipped(bool f);
	bool isKicking();
	void setKicking(float t);
	void setGroundLevel(float level);
	void activateRunningMechanics(bool isRunner);
	bool isRunnerLevel();
	void setLevelEdges(float left, float right);


	

	sf::Texture dino;
	Input in;
	Animation walk;
	Animation damaged;
	Animation kick;
	Animation* currentAnimation;
	



	//gravity and jump components
	float scale;
	bool isJumping;
	sf::Vector2f stepVelocity;
	sf::Vector2f gravity;
	sf::Vector2f jumpVector;
	float jumpHoldTime;
	float maxJumpHoldTime;
	float jumpForce;
	float jumpHoldForce;
	float groundLevel;
	bool runnerLevel;
	bool onGround;


	//kick compnents
	float kickTime = 0.f;
	float kickTimeElapsed = 0.f;

	//damage components
	float damagedTimer = 0.f;
	float damageLength = 100.0f;

	//edge of RunnerLevel variables
	float boundLeft;
	float boundRight;
};

