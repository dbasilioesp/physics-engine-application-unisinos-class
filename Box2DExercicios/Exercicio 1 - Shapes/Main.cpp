#include "Render.h"
#include <GL/glut.h>
#include <cstdio>
#include <iostream>


int32 framePeriod = 16;
int32 mainWindow;
int32 windowHeight=450, windowWidth=450;
float32 timeStep;
int32 velocityIterations;
int32 positionIterations;
b2Vec2 viewCenter(0.0f, 0.0f);
float32 viewZoom = 1.0f;
b2World *world;
b2Body *box1, *box2, *circle1, *circle2, *line1, *line2;
b2Body *ground;
DebugDraw renderer;


b2Body* CreateBox(float posX, float posY, 
				  float32 widthRay=5.0, float32 heightRay=5.0, 
				  float32 density=10.0, float32 friction=0.5, float32 restitution=0.5)
{
	//Primeiro, criamos a definição do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	//Estamos usando uma forma de poligono, que pode ter até 8 vértices
	b2PolygonShape shape;
	shape.SetAsBox(widthRay, heightRay);

	//Depois, criamos uma fixture que vai conter a forma do corpo
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;

	b2Body *box;
	box =  world->CreateBody(&bodyDef);	
	box->CreateFixture(&fixture);

	return box;
}


b2Body* CreateCircle(float posX, float posY, float32 radius=5.0, 
					 float32 density=10.0, float32 friction=0.5, float32 restitution=0.5)
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


b2Body* CreateEdge(float posX, float posY, const b2Vec2 &v1, const b2Vec2 &v2, 
					 float32 density=10.0, float32 friction=0.5, float32 restitution=0.5)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	b2EdgeShape shape;
	shape.Set(v1, v2);
	
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;

	b2Body *line;
	line =  world->CreateBody(&bodyDef);
	line->CreateFixture(&fixture);

	return line;
}


b2Body* CreatePolygon(float posX, float posY, b2Vec2 *vertices, int32 count,
					   float32 density=10.0, float32 friction=0.5, float32 restitution=0.5){

	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	b2PolygonShape shape;
	shape.Set(vertices, count);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;
	
	b2Body *polygon;
	polygon = world->CreateBody(&bodyDef);
	polygon->CreateFixture(&fixture);

	return polygon;
}


//Resize callback -- GLUT
void Resize(int32 width, int32 height)
{	
	// Evita a divisao por zero
	if(height == 0) 
		height = 1;

	GLfloat aspectRadio = width/height;
	
	// Especifica as dimensões da Viewport
	glViewport(0, 0, windowWidth, windowHeight);

	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Estabelece a janela de seleção (esquerda, direita, inferior, 
	// superior) mantendo a proporção com a janela de visualização
	if (width <= height) 
		gluOrtho2D (-40.0f, 40.0f, -40.0f*aspectRadio, 40.0f*aspectRadio);
	else 
		gluOrtho2D (-40.0f*aspectRadio, 40.0f*aspectRadio, -40.0f, 40.0f);
}

// Timer callback -- para tentar deixar o framerate em 60Hz
// This is used to control the frame rate (60Hz).
void Timer(int)
{
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 0);
}


//Função de inicialização da Box2D
void InitBox2D()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -9.8f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);

	// Define os parâmetro para a simulação
	// Quanto maior, mais preciso, porém, mais lento
    velocityIterations = 6;
    positionIterations = 2;
	timeStep = 1.0f / 60.0f; //60 Hz
}


// Função de Execução da Simulação
void RunBox2D()
{
	world->Step(timeStep, velocityIterations, positionIterations);
	world->ClearForces();
}


//Calback de rendering, que também chama o passo da simulação
void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RunBox2D();

	glColor3f(1,0,0);
	glPointSize(5);

	//PERCORRE A LISTA DE CORPOS RÍGIDOS DO MUNDO E CHAMA A ROTINA DE DESENHO PARA A LISTA DE FIXTURES DE CADA UM
	b2Color color; color.r = 1.0; color.g = 0.0; color.b = 0.0;
	b2Body *b;
	for(b = world->GetBodyList(); b; b=b->GetNext())
	{
		renderer.DrawFixture(b->GetFixtureList(), color);
	}
		
	glutSwapBuffers();
}


//Callback de teckado
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	}
}


int main(int argc, char** argv)
{
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(windowWidth, windowHeight);
	mainWindow = glutCreateWindow(title);
	glutDisplayFunc(SimulationLoop);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);	
	glutTimerFunc(framePeriod, Timer, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	InitBox2D();

	box1 = CreateBox(-10, 15, 10, 10, 0, 0, 0.2);
	box2 = CreateBox(10, 10);
	box2->SetTransform(box2->GetWorldCenter(), -10);
	circle1 = CreateCircle(-10, -15);
	circle2 = CreateCircle(10, 30);
	line1 = CreateEdge(0, 25, b2Vec2(-20.0, 0.0), b2Vec2(20.0, 0.0));

	b2Vec2 vertices[3];
	vertices[0].Set(0.0f, 0.0f);
	vertices[1].Set(5.0f, 0.0f);
	vertices[2].Set(0.0f, 5.0f);
	int32 count = 3;
	CreatePolygon(20.0f, 20.0f, vertices, count);

	//Cria o chão
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape,1.0);

	glutMainLoop();

	return 0;
}
