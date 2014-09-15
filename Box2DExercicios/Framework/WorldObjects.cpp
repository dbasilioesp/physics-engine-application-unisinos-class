#include "WorldObjects.h"


void Create4Walls(b2World *world){

	b2BodyDef bodyDef;
	b2EdgeShape edgeShape;

	{
		//Cria o chão
		edgeShape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
		b2Body *ground = world->CreateBody(&bodyDef);
		ground->CreateFixture(&edgeShape,0.0);
	}
	
	{
		//Cria o teto
		edgeShape.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
		b2Body *roof = world->CreateBody(&bodyDef);
		roof->CreateFixture(&edgeShape,0.0);
	}

	{
		//Cria a parede esquerda
		edgeShape.Set(b2Vec2(-39.5, 39.5), b2Vec2(-39.5, -39.5));
		b2Body *leftWall = world->CreateBody(&bodyDef);
		leftWall->CreateFixture(&edgeShape,0.0);
	}

	{
		//Cria a parede direita
		edgeShape.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
		b2Body *rightWall = world->CreateBody(&bodyDef);
		rightWall->CreateFixture(&edgeShape,0.0);
	}

}


b2Body *CreateBox(b2World *world, float posX, float posY, float width, float height, float massa, float coefatrito, float coefrestituicao)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX,posY);
	bodyDef.type = b2_dynamicBody;
	
	b2PolygonShape box;
	box.SetAsBox(width/2, height/2);
	
	b2FixtureDef f;
	f.shape = &box;
	f.density = massa/(width*width);
	f.friction = coefatrito;
	f.restitution = coefrestituicao;

	b2Body *object;
	object = world->CreateBody(&bodyDef);
	object->CreateFixture(&f);
	
	return object;
}


b2Body *CreateCircle(b2World *world, float posX, float posY, float32 radius, 
					 float32 density, float32 friction, float32 restitution)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	b2CircleShape shape;
	shape.m_radius = radius;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;

	b2Body *circle;
	circle =  world->CreateBody(&bodyDef);
	circle->CreateFixture(&fixture);

	return circle;
}
