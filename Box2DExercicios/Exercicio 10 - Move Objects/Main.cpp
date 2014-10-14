#include "Render.h"
#include "ForceFunctions.h"
#include "WorldObjects.h"
#include "Settings.h"
#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include <vector>


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
b2Body *bodySelected;
vector<b2Body*> bodies;
int32 selectedIndex = 0;


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

	renderer.DrawString(10, 20, "[b] Box");
	renderer.DrawString(10, 40, "[c] Circle");
	renderer.DrawString(10, 60, "[w,a,s,d] Move");
	renderer.DrawString(10, 80, "[n] Next");
		
	glutSwapBuffers();

}

// Rotina de callback de teclado da GLUT
void Keyboard(unsigned char key, int x, int y)
{
	b2Body *body;
	b2Vec2 force;
	int32 size;

	switch (key)
	{
	case 'b':
		body = CreateBox(world, 0.0, 0.0, 7.0, 7.0, 3.0, 0.5, 0.5);
		if(bodySelected == NULL)
			bodySelected = body;
		bodies.push_back(body);
		break;
	case 'c':
		body = CreateCircle(world, 0.0, 0.0, 3.0f, 0.7f);
		if(bodySelected == NULL)
			bodySelected = body;
		bodies.push_back(body);
		break;
	case 'n':
		size = bodies.size();
		if(size > 0){
			if(selectedIndex >= size){
				selectedIndex = 0;
				bodySelected = bodies[0];
			}else{
				for (int i = 0; i < size; i++){
					if(i == selectedIndex){
						bodySelected = bodies[i];
						selectedIndex += 1;
						break;
					}
				}
			}
		}
		break;
    //Sai do programa
	case 27:
		world->~b2World();
		system("exit");
		exit(0);
		break;
	}

	if(bodySelected != NULL){
		switch(key)
		{
		case 'w':
			force = CalculaComponentesDoVetor(1000, 90);
			bodySelected->ApplyForceToCenter(force, true);
			break;
		case 's':
			force = CalculaComponentesDoVetor(1000, -90);
			bodySelected->ApplyForceToCenter(force, true);
			break;
		case 'd':
			force = CalculaComponentesDoVetor(1000, 0);
			bodySelected->ApplyForceToCenter(force, true);
			break;
		case 'a':
			force = CalculaComponentesDoVetor(1000, 180);
			bodySelected->ApplyForceToCenter(force, true);
			break;
		}
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