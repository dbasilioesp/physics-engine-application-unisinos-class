#pragma once
#include "Box2D\Box2D.h"
#include "BodyUserData.h"


void Create4Walls(b2World *world, float width=39.5, float height=39.5);

b2Body *CreateBox(b2World *world, float posX, float posY, float width, float height, 
				  float density, float friction, float restitution, BodyUserData *userData = NULL);

b2Body *CreateCircle(b2World *world, float posX, float posY, float32 radius=5.0, 
					 float32 density=10.0, float32 friction=0.5, float32 restitution=0.5, BodyUserData *userData = NULL);

b2Body *CreateWoodBarTall(b2World *world, float posX, float posY, 
					 float32 density=3.0, float32 friction=0.4, float32 restitution=0.5, BodyUserData *userData = NULL);

b2Body *CreateWoordBarLarge(b2World *world, float posX, float posY, 
					 float32 density=3.0, float32 friction=0.4, float32 restitution=0.5, BodyUserData *userData = NULL);