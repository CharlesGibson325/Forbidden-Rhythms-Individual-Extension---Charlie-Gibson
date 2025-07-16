#pragma once

#include "Framework/GameObject.h"
#include "Framework/Animation.h"

class Seagull : public GameObject
{
    public:  
       Seagull();  
       ~Seagull();  

       void update(float dt, float windowWidth); // Ensure this method is public  

	    sf::Texture seagullTexture;
	    Animation fly;


        // For Egg
        GameObject egg;
        bool carryingEgg = false;
        float eggDropTimer = 0.f;
        float eggDropInterval = 2.f;
		bool eggReadyToDrop = false;
		float eggCarryTimer = 0.f;
        sf::Texture eggTexture;
};

