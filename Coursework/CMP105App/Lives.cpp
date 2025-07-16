#include "Lives.h"

Lives::Lives()
{
	
	setPosition(sf::Vector2f(30, 100));
	setSize(sf::Vector2f(180, 60));
	hearts.loadFromFile("gfx/Runner_Lives.png");
	setTexture(&hearts);

	lives3.addFrame(sf::IntRect(0, 0, 90, 30));
	lives2.addFrame(sf::IntRect(0, 30, 90, 30));
	lives1.addFrame(sf::IntRect(0, 60, 90, 30));
	lives0.addFrame(sf::IntRect(0, 90, 90, 30));
	currentAnimation = &lives3;
	maxLives = 3;
	currentLives = maxLives;
}

Lives::~Lives()
{
}

void Lives::takeDamage()
{
	if (currentLives == 3)
	{
		currentLives = 2;
		currentAnimation = &lives2;
	}

	else if (currentLives == 2)
	{
		currentLives = 1;
		currentAnimation = &lives1;
	}

	else if (currentLives == 1)
	{
		currentLives = 0;
		currentAnimation = &lives0;
	}

}

void Lives::resetLives()
{
	currentLives = maxLives;
	currentAnimation = &lives3;
}

void Lives::update(float dt) {
	currentAnimation->animate(dt);
	setTextureRect(currentAnimation->getCurrentFrame());
}

const bool Lives::isDead() {
	return currentLives <= 0;
}