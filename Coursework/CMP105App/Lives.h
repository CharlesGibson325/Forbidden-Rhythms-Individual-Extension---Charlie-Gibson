#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"
class Lives : public GameObject
{ 
public:
	Lives();
	~Lives();
	
	sf::Texture hearts;
	sf::RenderWindow* window;
	Input* in;

	int currentLives;
	int maxLives;
	

	void takeDamage();
	void resetLives();
	void update(float dt);
	const bool isDead();

	Animation lives3;
	Animation lives2;
	Animation lives1;
	Animation lives0;
	Animation* currentAnimation;
};

