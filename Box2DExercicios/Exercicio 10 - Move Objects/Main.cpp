#include "Render.h"
#include "ForceFunctions.h"
#include "WorldObjects.h"
#include "Settings.h"
#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include <vector>


using namespace std;


namespace
{
	int32 framePeriod = 16; //milliseconds
	int32 height = 450;
	int32 width = 450;
	int32 mainWindow;
	float32 timeStep;
	int32 velocityIterations ;
	int32 positionIterations ;
	b2Vec2 viewCenter(0.0f, 0.0f);
	b2World *world;
	b2Body *bodySelected;
	// Objeto com as rotinas de renderização dos objetos
	DebugDraw renderer;
};


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

	//Tire a gravidade! ;)
	//b2Vec2 gravity(0.0f, 0.0f);

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


// Função que imprime todos os objetos  
// armazenados na Box2D
void PrintBodies()
{
	b2Body *b;
	float ang;
	b2Vec2 pos;
	for(b = world->GetBodyList(); b; b=b->GetNext())
	{
		pos = b->GetPosition();
		ang = b->GetAngle();
		printf("%4.2f %4.2f %4.2f\n", pos.x, pos.y, ang);	
	}
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
	PrintBodies();

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

//Rotina de callback de teclado da GLUT
void Keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'b':
		CreateBox(world, 0.0, 0.0, 7.0, 7.0, 3.0, 0.5, 0.5);
		break;
	case 'c':
		CreateCircle(world, 0.0, 0.0);
		break;
	case 'n':
	
		break;
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
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(height, width);
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d -- Aprendendo Forças", b2_version.major, b2_version.minor, b2_version.revision);
	mainWindow = glutCreateWindow(title);

	glutDisplayFunc(SimulationLoop);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	//Usa um timer para controlar o frame rate.
	glutTimerFunc(framePeriod, Timer, 1);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//Rotina com a inicialização do mundo
	InitBox2D();

	Create4Walls(world);
	
	glutMainLoop();

	return 0;
}