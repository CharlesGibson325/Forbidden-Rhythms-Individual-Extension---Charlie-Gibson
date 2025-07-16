#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player()
{
	setPosition(sf::Vector2f( 100, 100));
	setSize(sf::Vector2f(100, 100));

	dino.loadFromFile("gfx/dinoVita.png");
	
	setTexture(&dino); // set that texture to this object.

	// dino texture is 24x24px. Frames for walk are 4 - 9, inclusive.
	for (int i = 4; i < 10; ++i)
	{
		walk.addFrame(sf::IntRect(i*24.f, 0.f, 24.f, 21.f));
	}
	for (int i = 0; i < 4; ++i)
	{
		kick.addFrame(sf::IntRect(i * 24.f + 240.f, 0.f, 24.f, 21.f));
	}
	damaged.addFrame(sf::IntRect(0, 0, 24, 21));
	damaged.addFrame(sf::IntRect(15*24, 0, 24, 21));
	damaged.setFrameSpeed(1.f / 4.f);
	currentAnimation = &walk;
	walk.setFrameSpeed(1.f / 10.f);
	groundLevel = 500.f; // Default value to be overridden per level


	velocity = { 0,0 };

	//Gravity and Jumping Values
	scale = 200.f;
	gravity = sf::Vector2f(0, 9.810f) * scale; // Gravity force
	jumpVector = sf::Vector2f(0, -4.0f) * scale; // Jump force
	isJumping = false;
	jumpHoldTime = 0.f;
	maxJumpHoldTime = 0.25f; // Max time (in seconds) you can hold jump for extra height
	jumpForce = -3.0f * scale; // Initial jump force
	jumpHoldForce = -12.0f * scale; // Extra force per second while holding space
}

Player::~Player()
{
}

void Player::setGroundLevel(float level)
{
	groundLevel = level;
}

const bool Player::isOnGround() {
	return getPosition().y + getSize().y >= groundLevel;
}


bool Player::isKicking()
{
	return kickTimeElapsed > 0 && kickTimeElapsed < kickTime;
}

void Player::setKicking(float t)
{
	kick.setFrameSpeed(t / kick.getSize());
	kick.reset();
	currentAnimation = &kick;
	kickTime = t;
	kickTimeElapsed = 0.f;
}

void Player::setFlipped(bool f)
{
	currentAnimation->setFlipped(f);
}

void Player::handleInput(float dt)
{
	if(input->isKeyDown(sf::Keyboard::Space)) {
		if (!isJumping && isOnGround()) {
			// Start jump
			stepVelocity.y = jumpForce;
			isJumping = true;
			jumpHoldTime = 0.f;
		}
		else if (isJumping && jumpHoldTime < maxJumpHoldTime) {
			// Continue to apply upward force while holding, up to a maximum height (maxJumpHoldTime)
			stepVelocity.y += jumpHoldForce * dt;
			jumpHoldTime += dt;
		}
	}

	if (input->isKeyDown(sf::Keyboard::D) || input->isKeyDown(sf::Keyboard::Right)) {
		// Move right
		stepVelocity.x = 0.3f * scale;
		currentAnimation = &walk;
	}
	else if (input->isKeyDown(sf::Keyboard::A) || input->isKeyDown(sf::Keyboard::Left)) {
		// Move left
		stepVelocity.x = -3.0f * scale;
		currentAnimation = &walk;
	}
	else {
		stepVelocity.x = 0.f; // Stop horizontal movement
	}
}


void Player::update(float dt)
{

	//Activate if RunnerLevel is playing
	if (runnerLevel)
	{
		//Gravity and Velocity Logic
		sf::Vector2f pos = stepVelocity * dt + 0.5f * gravity * dt * dt; //ut +1/2at^2
		stepVelocity += gravity * dt; //v = u + at
		setPosition(getPosition() + pos);

		//check for collision with ground
		if (isOnGround())
		{
			// If on ground, reset jump state and position
			isJumping = false;
			jumpHoldTime = 0.f;
			stepVelocity.y = 0.f;
			setPosition(getPosition().x, groundLevel - getSize().y);
		}

		//keeps Player in the level
		sf::Vector2f pos2 = getPosition();
		float width = getSize().x;
		if (pos2.x < boundLeft)
			pos2.x = boundLeft;
		if (pos2.x + width > boundRight)
			pos2.x = boundRight - width;
		setPosition(pos2);
	}
	


	if (currentAnimation == &damaged)
		damagedTimer += dt;
	if (damagedTimer > damageLength)
	{
		currentAnimation = &walk;
		damagedTimer = 0.f;
	}


	// check for kick status.
	if (kickTime != 0)
	{
		if (kickTimeElapsed < kickTime) kickTimeElapsed += dt;
		else
		{
			currentAnimation = &walk;
			kickTime = 0.f;
			kickTimeElapsed = 0.f;
		}
	}

	currentAnimation->animate(dt);
	setTextureRect(currentAnimation->getCurrentFrame());

}


bool Player::isDamaged()
{
	return currentAnimation == &damaged;
}

void Player::setDamaged(float timer)
{
	currentAnimation = &damaged;
	damageLength = timer;
}

//Code to ensure gravity is not applied when not in RunnerLevel
void Player::activateRunningMechanics(bool isRunner)
{
	runnerLevel = true;
}

bool Player::isRunnerLevel()
{
	return runnerLevel;
}

//Defines the level Edges
void Player::setLevelEdges(float left, float right)
{
	boundLeft = left;
	boundRight = right;
}