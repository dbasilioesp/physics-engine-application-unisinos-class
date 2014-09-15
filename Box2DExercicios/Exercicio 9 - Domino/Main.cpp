#include "Render.h"
#include "ForceFunctions.h"
#include "WorldObjects.h"
#include "Settings.h"
#include <GL/glut.h>
#include <cstdio>
#include <iostream>


using namespace std;


int32 framePeriod = 16; //milliseconds
int32 height = 450;
int32 width = 450;
int32 mainWindow;
float32 timeStep;
int32 velocityIterations ;
int32 positionIterations ;
b2Vec2 viewCenter(0.0f, 0.0f);
b2World *world;
DebugDraw renderer;  // Objeto com as rotinas de renderização dos objetos


void Domino(){

	b2Body * boxCreated;
	for (int i = 10; i > 0; i--){
		boxCreated = CreateBox(world, ((i*4)*2)-35.0, -35.0, 2.0, 10.0, 3.0, 0.5, 0.5);
	}

	b2Vec2 force;
	force = CalculaComponentesDoVetor(200, 0);
	boxCreated->ApplyForce(force, b2Vec2(-10.0, 4.0), true);
}


//Rotina de Callback de redimensionamento da janela 
void Resize(int32 w, int32 h)
{
	ResizeWithProportion(w, h, -40.0, 40.0, -40.0, 40.0);
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
    velocityIterations = 8;
    positionIterations = 3;
	timeStep = 1.0f / 60.0f;
}

// Função de Execução da Simulação
void RunBox2D()
{
	world->Step(timeStep, velocityIterations, positionIterations);
	world->ClearForces();
}


// Callback de temporizador da GLUT (pra não ficar muito rápido)
void Timer(int)
{
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 1);
}

//Callback de desenho da GLUT, nela é chamada a rotina que chama o passo da simulação
void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RunBox2D();
	
	glColor3f(1,0,0);
	glPointSize(5);

	b2Color color; color.r = 1.0; color.g = 0.0; color.b = 0.0;

	b2Body *b;
	for(b = world->GetBodyList(); b; b=b->GetNext())
	{
		renderer.DrawFixture(b->GetFixtureList(),color);
	}
		
	glutSwapBuffers();

}

//Rotina de callback de teclado da GLUT
void Keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{
    //Sai do programa
	case 27:
		world->~b2World();
		system("exit");
		exit(0);
		break;
	}

	glutPostRedisplay();
}

//Main :)
int main(int argc, char** argv)
{
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d -- Domino", b2_version.major, b2_version.minor, b2_version.revision);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(height, width);
	mainWindow = glutCreateWindow(title);
	glutDisplayFunc(SimulationLoop);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(framePeriod, Timer, 1);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//Rotina com a inicialização do mundo
	InitBox2D();

	Create4Walls(world);
	Domino();

	glutMainLoop();

	return 0;
}