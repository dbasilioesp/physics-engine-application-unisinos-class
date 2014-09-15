#include "Render.h"
#include "ForceFunctions.h"
#include "WorldObjects.h"
#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include <sstream>


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
float lauchAngle = 45;
b2Body *boxes[100];
int counterBoxes = 0;



void DesenhaLinhaGuia()
{
		glColor3f(0,0,1);
		b2Vec2 pInicial(-35,-35);
		b2Vec2 pFinal = CalculaComponentesDoVetor(10.0, lauchAngle);

		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2d(pInicial.x,pInicial.y);
		glVertex2d(pInicial.x + pFinal.x, pInicial.y + pFinal.y);
		glEnd();
		glLineWidth(1);
}


//Rotina de Callback de redimensionamento da janela 
void Resize(int32 w, int32 h)
{
	GLsizei width, height;
	
	// Evita a divisao por zero
	if(h == 0) h = 1;

	// Atualiza as variáveis
	width = w;
	height = h;

	// Especifica as dimensões da Viewport
	glViewport(0, 0, width, height);

	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Estabelece a janela de seleção (esquerda, direita, inferior, 
	// superior) mantendo a proporção com a janela de visualização
	if (width <= height) 
		gluOrtho2D (-40.0f, 40.0f, -40.0f*height/width, 40.0f*height/width);
	else 
		gluOrtho2D (-40.0f*width/height, 40.0f*width/height, -40.0f, 40.0f);
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

	ostringstream aux1, aux2;  //incluir sstream
	aux1 << "Angulo = " << lauchAngle;
	renderer.DrawString(10,20,aux1.str().c_str());
	aux2 << "Comandos: space, w, s ";
	renderer.DrawString(10,40,aux2.str().c_str());

	DesenhaLinhaGuia();
		
	glutSwapBuffers();

}

//Rotina de callback de teclado da GLUT
void Keyboard(unsigned char key, int x, int y)
{
	switch (key){
	case ' ':
		{

		boxes[counterBoxes] = CreateBox(world,-35,-35.0,2.0,2.0,1.0,0.5,0.5);
		
		b2Vec2 vetorForca;
		vetorForca = CalculaComponentesDoVetor(1500, lauchAngle);
		
		boxes[counterBoxes]->ApplyForceToCenter(vetorForca,true);

		if (counterBoxes < 99)
			counterBoxes++;
		else
		{
			for(int i = 0; i < counterBoxes; i++)
				world->DestroyBody(boxes[i]);
			counterBoxes = 0;
		}

		break;
		}
	case 'w':
		lauchAngle += 1;
		break;
	case 's':
		lauchAngle -= 1;
		break;
    case 27: //Sai do programa
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
	sprintf(title, "Box2D Version %d.%d.%d -- Aprendendo Forças", b2_version.major, b2_version.minor, b2_version.revision);
	
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

	glutMainLoop();

	return 0;
}