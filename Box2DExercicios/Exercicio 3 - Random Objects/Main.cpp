
#include "Render.h"
#include <GL/glut.h>

#include <cstdio>
#include <iostream>
#include <ctime>

using namespace std;

//Algumas globais para interface e simulação
int32 framePeriod = 16;
int32 mainWindow;
float scaleFactor;
int altura=450, largura=450;
float32 timeStep;
int32 velocityIterations ;
int32 positionIterations ;
int tx, ty, tw, th;
b2Vec2 viewCenter(0.0f, 0.0f);
float32 viewZoom = 1.0f;

// O objeto World serve para armazenar os dados da simulação --> MUNDO FÍSICO DA BOX2D
b2World *world;

//Alguns corpos rígidos
b2Body* ground;

//Objeto para a classe que faz o desenho das formas de colisão dos corpos rígidos
DebugDraw renderer;


b2Body *CreateBox(float posX, float posY, 
				  float32 widthRay=5.0, float32 heightRay=5.0, 
				  float32 density=10.0, float32 friction=0.5, float32 restitution=0.5)
{
	if(widthRay <= 0)
		widthRay = 1;

	if(heightRay <= 0)
		heightRay = 1;

	b2Body *box;

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
	//Setamos outras propriedades da fixture
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;

	//Por fim, criamos o corpo...
	box =  world->CreateBody(&bodyDef);

	//... e criamos a fixture do corpo 	
	box->CreateFixture(&fixture);

	return box;
}


b2Body *CreateCircle(float posX, float posY, float32 radius=5.0, 
					 float32 density=10.0, float32 friction=0.5, float32 restitution=0.5)
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


b2Body *CreateEdge(float posX, float posY, const b2Vec2 &v1, const b2Vec2 &v2, 
					 float32 density=10.0, float32 friction=0.5, float32 restitution=0.5)
{
	b2Body *line;

	//Primeiro, criamos a definição do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	//Estamos usando uma forma de poligono, que pode ter até 8 vértices
	b2EdgeShape shape;
	shape.Set(v1, v2);
	
	//Depois, criamos uma fixture que vai conter a forma do corpo
	b2FixtureDef fixture;
	fixture.shape = &shape;

	//Setamos outras propriedades da fixture
	fixture.density = density;
	fixture.friction = friction;
	fixture.restitution = restitution;

	//Por fim, criamos o corpo...
	line =  world->CreateBody(&bodyDef);

	//... e criamos a fixture do corpo 	
	line->CreateFixture(&fixture);

	return line;
}




//Resize callback -- GLUT
void Resize(int32 w, int32 h)
{
	GLsizei largura, altura;
	
	// Evita a divisao por zero
	if(h == 0) h = 1;

	// Atualiza as variáveis
	largura = w;
	altura = h;

	// Especifica as dimensões da Viewport
	glViewport(0, 0, largura, altura);

	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Estabelece a janela de seleção (esquerda, direita, inferior, 
	// superior) mantendo a proporção com a janela de visualização
	if (largura <= altura) 
		gluOrtho2D (-40.0f, 40.0f, -40.0f*altura/largura, 40.0f*altura/largura);
	else 
		gluOrtho2D (-40.0f*largura/altura, 40.0f*largura/altura, -40.0f, 40.0f);
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


// Função que imprime todos os objetos  
void PrintBodies()
{
	b2Body *b;
	float ang;
	b2Vec2 pos;
	//PERCORRE A LISTA DE CORPOS RÍGIDOS DO MUNDO
	for(b = world->GetBodyList(); b; b=b->GetNext())
	{
		pos = b->GetPosition();
		ang = b->GetAngle();
		printf("%4.2f %4.2f %4.2f\n", pos.x, pos.y, ang);	
	}
}

//Calback de rendering, que também chama o passo da simulação
void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RunBox2D();
	// PrintBodies();

	b2Body *b;
	glColor3f(1,0,0);
	glPointSize(5);

	b2Color color; color.r = 1.0; color.g = 0.0; color.b = 0.0;

	//PERCORRE A LISTA DE CORPOS RÍGIDOS DO MUNDO E CHAMA A ROTINA DE DESENHO PARA A LISTA DE FIXTURES DE CADA UM
	for(b = world->GetBodyList(); b; b=b->GetNext())
	{
		renderer.DrawFixture(b->GetFixtureList(),color);
	}
		
	glutSwapBuffers();

	
}

//Callback de teckado
void Keyboard(unsigned char key, int x, int y)
{
	int widthRay;
	int heightRay;
	int radius;
	int x1;
	int x2;
	int x3;
	int x4;
	int posX = 40 - rand() % 80;
	int posY = 40 - rand() % 80;
	int density = rand() % 10;
	int friction = (rand() % 10) / 10;
	int restitution = (rand() % 10) / 10;

	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'b':
		widthRay = rand() % 10;
		heightRay = rand() % 10;
		CreateBox(posX, posY, widthRay, heightRay, density, friction, restitution);
		break;
	case 'c':
		radius = rand() % 10;
		CreateCircle(posX, posY, radius, density, friction, restitution);
		break;
	case 'l':
		x1 = 20 - rand() % 40;
		x2 = 20 - rand() % 40;
		x3 = 20 - rand() % 40;
		x4 = 20 - rand() % 40;
		CreateEdge(posX, posY, b2Vec2(x1, x2), b2Vec2(x3, x4), density, friction, restitution);
		break;
	}
}


void Pause(int)
{
	//settings.pause = !settings.pause;
}


int main(int argc, char** argv)
{
	srand(time(0));
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(altura, largura);
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
	mainWindow = glutCreateWindow(title);
	//glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(SimulationLoop);

	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);	
	glutTimerFunc(framePeriod, Timer, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	InitBox2D();

	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape,1.0);

	glutMainLoop();

	return 0;
}
