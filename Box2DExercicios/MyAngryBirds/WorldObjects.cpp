#include "WorldObjects.h"


void Create4Walls(b2World *world, float width, float height){

	b2BodyDef bodyDef;
	b2EdgeShape edgeShape;
	BodyUserData *userData;
	
	userData = new BodyUserData();
	userData->entityType = "scenary";

	{
		//Cria o chão
		edgeShape.Set(b2Vec2(-width, -height), b2Vec2(width, -height));
		b2Body *ground = world->CreateBody(&bodyDef);
		ground->CreateFixture(&edgeShape,0.0);
		ground->SetUserData((void*)userData);
	}
	
	{
		//Cria o teto
		edgeShape.Set(b2Vec2(-width, height), b2Vec2(width, height));
		b2Body *roof = world->CreateBody(&bodyDef);
		roof->CreateFixture(&edgeShape,0.0);
		roof->SetUserData((void*)userData);
	}

	{
		//Cria a parede esquerda
		edgeShape.Set(b2Vec2(-width, height), b2Vec2(-width, -height));
		b2Body *leftWall = world->CreateBody(&bodyDef);
		leftWall->CreateFixture(&edgeShape,0.0);
		leftWall->SetUserData((void*)userData);
	}

	{
		//Cria a parede direita
		edgeShape.Set(b2Vec2(width, height), b2Vec2(width, -height));
		b2Body *rightWall = world->CreateBody(&bodyDef);
		rightWall->CreateFixture(&edgeShape,0.0);
		rightWall->SetUserData((void*)userData);
	}

}


b2Body *CreateBox(b2World *world, float posX, float posY, float width, float height, 
				  float density, float friction, float restitution, BodyUserData *userData)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX,posY);
	bodyDef.type = b2_dynamicBody;
	
	b2PolygonShape box;
	box.SetAsBox(width/2, height/2);
	
	b2FixtureDef f;
	f.shape = &box;
	f.density = density/(width*width);
	f.friction = friction;
	f.restitution = restitution;

	b2Body *object;
	object = world->CreateBody(&bodyDef);
	object->CreateFixture(&f);
	
	if(userData == NULL){
		userData = new BodyUserData();
		userData->entityType = "object";
		userData->health = 1.00;
	}

	object->SetUserData((void*)userData);

	return object;
}


b2Body *CreateCircle(b2World *world, float posX, float posY, float32 radius, 
					 float32 density, float32 friction, float32 restitution, BodyUserData *userData)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;
	bodyDef.angularDamping = 1.0;

	b2CircleShape shape;
	shape.m_radius = radius;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	float area = b2_pi * radius * radius;
	fixture.density = density/area;
	fixture.friction = friction;
	fixture.restitution = restitution;

	b2Body *circle;
	circle =  world->CreateBody(&bodyDef);
	circle->CreateFixture(&fixture);

	if(userData == NULL){
		userData = new BodyUserData();
		userData->entityType = "object";
		userData->health = 1.00;
	}

	circle->SetUserData((void*)userData);
	
	return circle;
}


b2Body *CreateWoodBarTall(b2World *world, float posX, float posY, BodyUserData *userData)
{
	b2Body *object = CreateBox(world, posX, posY, 3, 25, 0.25, 0.4, 0.5);
	
	if(userData == NULL){
		userData = new BodyUserData();
		userData->entityType = "object";
		userData->health = 2.00;
	}

	object->SetUserData((void*)userData);

	return object;
}


b2Body *CreateWoodBarLarge(b2World *world, float posX, float posY, BodyUserData *userData)
{
	b2Body *object = CreateBox(world, posX, posY, 25, 3, 0.25, 0.4, 0.5);
	
	if(userData == NULL){
		userData = new BodyUserData();
		userData->entityType = "object";
		userData->health = 1.00;
	}

	object->SetUserData((void*)userData);

	return object;
}