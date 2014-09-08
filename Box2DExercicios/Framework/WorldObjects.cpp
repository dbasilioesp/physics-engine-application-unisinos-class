#include "WorldObjects.h"


void Create4Walls(b2World *world){

	b2BodyDef bd;
	b2EdgeShape shape;

	//Cria o chão
	{
		b2Body *ground = world->CreateBody(&bd);
		shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
		ground->CreateFixture(&shape,0.0);
	}
	
	//Cria o teto
	{
		b2Body *roof = world->CreateBody(&bd);
		shape.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
		roof->CreateFixture(&shape,0.0);
	}

	//Cria a parede esquerda
	{
		b2Body *leftWall = world->CreateBody(&bd);
		shape.Set(b2Vec2(-39.5, 39.5), b2Vec2(-39.5, -39.5));
		leftWall->CreateFixture(&shape,0.0);
	}

	//Cria a parede direita
	{
		b2Body *rightWall = world->CreateBody(&bd);
		shape.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
		rightWall->CreateFixture(&shape,0.0);
	}

}


b2Body *CreateBox(b2World *world, float posX, float posY, float altura, float largura, float massa, float coefatrito, float coefrestituicao)
{
	//Cria o novo objeto (objeto novoObjeto)
	b2Body *object;
	b2BodyDef bd;
	
	bd.position.Set(posX,posY);
	bd.type = b2_dynamicBody;
	
	//2º passo: criação do corpo pelo mundo (mundo cria corpo)
	object = world->CreateBody(&bd);
	
	//3º passo: criação da definição da forma (b2PolygonShape, b2CircleShape ou b2EdgeShape)
	b2PolygonShape box;
	box.SetAsBox(largura/2, altura/2);
	
	//4º passo: criação da definição da fixture (b2FixtureDef)
	//Não esquecer de associar a forma com a fixture!
	b2FixtureDef f;
	f.shape = &box;
	f.density = massa/(altura*largura);
	f.friction = coefatrito;
	f.restitution = coefrestituicao;
	
	//5º passo: criação da fixture pelo corpo (objeto cria fixture)
	object->CreateFixture(&f);
	
	return object;
}


b2Body *CreateCircle(b2World *world, float posX, float posY, float32 radius, 
					 float32 density, float32 friction, float32 restitution)
{
	b2Body *circle;

	//Primeiro, criamos a definição do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	//Estamos usando uma forma de poligono, que pode ter até 8 vértices
	b2CircleShape shape;
	shape.m_radius = radius;

	//Depois, criamos uma fixture que vai conter a forma do corpo
	b2FixtureDef fixture;
	fixture.shape = &shape;

	//Setamos outras propriedades da fixture
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;

	//Por fim, criamos o corpo...
	circle =  world->CreateBody(&bodyDef);

	//... e criamos a fixture do corpo 	
	circle->CreateFixture(&fixture);

	return circle;
}
