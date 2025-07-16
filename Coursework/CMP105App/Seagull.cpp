#include "Seagull.h"
#include <iostream>

Seagull::Seagull()
{
	setPosition(sf::Vector2f(100, 100));
	setSize(sf::Vector2f(100, 100));
	seagullTexture.loadFromFile("gfx/Seagull.png");
	setTexture(&seagullTexture);

	// seagull texture is 25x25px. Frames for walk are 1 - 8, inclusive.
	for (int i = 0; i < 8; ++i)
	{
		fly.addFrame(sf::IntRect(i * 25, 0, 25, 25));
	}

	fly.setFrameSpeed(1.f / 5.f);

    //set up egg
	egg.setSize(sf::Vector2f(40, 40));
	eggTexture.loadFromFile("gfx/Egg.png");
	egg.setTexture(&eggTexture);
	egg.setAlive(false);
}

Seagull::~Seagull()
{
}

void Seagull::update(float dt, float windowWidth)  
{  
   sf::Vector2f pos = getPosition();  
   sf::Vector2f vel = getVelocity();  
   float width = getSize().x;  

   // Bounce at left edge  
   if (pos.x <= 0.0f + windowWidth * 0.25) {
       setVelocity(std::abs(vel.x), 0.0f);
	   fly.setFlipped(true);
   }  
   // Bounce at right edge  
   else if (pos.x + width >= windowWidth * 0.9) {  
       setVelocity(-std::abs(vel.x), 0.0f);
       fly.setFlipped(false);
   }  

   // Move seagull  
   setPosition(pos + vel * dt);  


   // Egg drop timer
   eggDropTimer += dt;
   if (!carryingEgg && eggDropTimer > eggDropInterval) {
       // Spawn egg under seagull, start carrying
       egg.setPosition(getPosition().x + getSize().x / 2 - egg.getSize().x / 2,
           getPosition().y + getSize().y);
       egg.setAlive(true);
       carryingEgg = true;
       eggReadyToDrop = false;
       eggCarryTimer = 0.f;
       eggDropTimer = 0.f;
       eggDropInterval = 2.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
   }

   // If carrying keep egg attached for 1 second
   if (carryingEgg && egg.isAlive() && !eggReadyToDrop) {
       eggCarryTimer += dt;
       // Keep egg under seagull
       egg.setPosition(getPosition().x + getSize().x / 2 - egg.getSize().x / 2,
           getPosition().y + getSize().y);
       if (eggCarryTimer >= 1.0f) {
           eggReadyToDrop = true;
       }
   }

   // If ready to drop, let egg fall
   if (carryingEgg && egg.isAlive() && eggReadyToDrop) {
       egg.move(0, 300 * dt); // simple gravity
   }
  

   fly.animate(dt);  
   setTextureRect(fly.getCurrentFrame());  
}