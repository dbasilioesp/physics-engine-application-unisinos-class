#pragma once
#include "Box2D\Box2D.h"


void Create4Walls(b2World *world, float width=39.5, float height=39.5);


b2Body *CreateBox(b2World *world, float posX, float posY, float width, float height, 
				  float density, float friction, float restitution);


b2Body *CreateCircle(b2World *world, float posX, float posY, float32 radius=5.0, 
					 float32 density=10.0, float32 friction=0.5, float32 restitution=0.5);