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
vector<b2Joint*> joints;

void CreateRobot()
{
	float posX = -10.0;
	float posY = -10.0;
	
	b2Body *head = CreateBox(world, 0.0 + posX, 0.0 + posY, 5.0, 7.0, 1.0, 0.5, 0.5);
	b2Body *body = CreateBox(world, 0.0 + posX, -8.0 + posY, 8.0, 8.0, 1.0, 0.5, 0.5);
	// Arms
	b2Body *leftArm = CreateBox(world, -8.0 + posX, -6.0 + posY, 7.0, 3.0, 1.0, 0.5, 0.5);
	b2Body *rightArm = CreateBox(world, 8.0 + posX, -6.0 + posY, 7.0, 3.0, 1.0, 0.5, 0.5);
	// Legs
	b2Body *leftLeg = CreateBox(world, -2.0 + posX, -16.0 + posY, 3.0, 8.0, 1.0, 0.5, 0.5);
	b2Body *rightLeg = CreateBox(world, 2.0 + posX, -16.0 + posY, 3.0, 8.0, 1.0, 0.5, 0.5);
	
	b2Body *leftWheel = CreateCircle(world, -2.0 + posX, -20.0 + posY, 1.5f);
	b2Body *rightWheel = CreateCircle(world, 2.0 + posX, -20.0 + posY, 1.5f);

	{
		// Head
		b2RevoluteJointDef rJointDef;
		rJointDef.Initialize(body, head, body->GetWorldPoint(b2Vec2(-0.0, 4.0)));
		rJointDef.collideConnected = true;
		
		joints.push_back(
			world->CreateJoint(&rJointDef)
		);
	}

	{
		b2RevoluteJointDef rJointDef;
		rJointDef.Initialize(body, leftArm, body->GetWorldPoint(b2Vec2(-4.0, 2.5)));
		rJointDef.collideConnected = false;
		
		joints.push_back(
			world->CreateJoint(&rJointDef)
		);
	}

	{
		b2RevoluteJointDef rJointDef;
		rJointDef.Initialize(body, rightArm, body->GetWorldPoint(b2Vec2(4.0, 2.5)));
		rJointDef.collideConnected = false;
		
		joints.push_back(
			world->CreateJoint(&rJointDef)
		);
	}

	{
		b2RevoluteJointDef rJointDef;
		rJointDef.Initialize(body, leftLeg, body->GetWorldPoint(b2Vec2(-2.0, -4.0)));
		rJointDef.collideConnected = false;
		
		rJointDef.lowerAngle = GrausParaRadianos(-35);
		rJointDef.upperAngle = GrausParaRadianos(35);
		rJointDef.enableLimit = true;

		joints.push_back(
			world->CreateJoint(&rJointDef)
		);
	}

	{
		b2RevoluteJointDef rJointDef;
		rJointDef.Initialize(body, rightLeg, body->GetWorldPoint(b2Vec2(2.0, -4.0)));
		rJointDef.collideConnected = true;
		
		rJointDef.lowerAngle = GrausParaRadianos(-35);
		rJointDef.upperAngle = GrausParaRadianos(35);
		rJointDef.enableLimit = true;

		joints.push_back(
			world->CreateJoint(&rJointDef)
		);
	}

	{
		b2WheelJointDef wJointDef;
		wJointDef.Initialize(leftLeg, leftWheel, 
							leftLeg->GetWorldPoint(b2Vec2(0.0, -3.5)), b2Vec2(0.0, 1.0));

		wJointDef.motorSpeed = -150.0f;
		wJointDef.maxMotorTorque = 150.0f;
		wJointDef.enableMotor = true;
		wJointDef.dampingRatio = 1.0;

		joints.push_back(
			world->CreateJoint(&wJointDef)
		);
	}
	{
		b2WheelJointDef wJointDef;
		wJointDef.Initialize(rightLeg, rightWheel, 
							rightLeg->GetWorldPoint(b2Vec2(0.0, -3.5)), b2Vec2(0.0, 1.0));

		wJointDef.motorSpeed = -150.0f;
		wJointDef.maxMotorTorque = 150.0f;
		wJointDef.enableMotor = true;
		wJointDef.dampingRatio = 1.0;

		joints.push_back(
			world->CreateJoint(&wJointDef)
		);
	}
}


void DrawJoint( b2Joint *joint, b2Color color)
{
	b2Vec2 anchor1, anchor2;
	anchor1 = joint->GetAnchorA();
	anchor2 = joint->GetAnchorB();

	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex2f(anchor1.x,anchor1.y);
	glVertex2f(anchor2.x,anchor2.y);
	glEnd();
	glPointSize(1);

	renderer.DrawSegment(anchor1, anchor2, color);
}


//Rotina de Callback de redimensionamento da janela 
void Resize(int32 w, int32 h)
{
	ResizeWithProportion(w, h, -40.0, 40.0, -40.0, 40.0);
}


//Função de inicialização da Box2D
void InitBox2D()
{
	b2Vec2 gravity(0.0f, -9.8f);

	world = new b2World(gravity);

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

// Callback de desenho da GLUT, nela é chamada a rotina que chama o passo da simulação
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
	for(b = world->GetBodyList(); b; b=b->GetNext()){
		renderer.DrawFixture(b->GetFixtureList(),color);
	}

	for (int i = 0; i < joints.size(); i++)
		DrawJoint(joints[i], color);
	
	glutSwapBuffers();

}

// Rotina de callback de teclado da GLUT
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
    case 27:  //Sai do programa
		{
			world->~b2World();
			system("exit");
			exit(0);
			break;
		}
	}

	glutPostRedisplay();
}

//Main :)
int main(int argc, char** argv)
{
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(height, width);
	mainWindow = glutCreateWindow(title);
	glutDisplayFunc(SimulationLoop);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(framePeriod, Timer, 1);  //Usa um timer para controlar o frame rate.

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	InitBox2D();

	Create4Walls(world);
	CreateRobot();

	glutMainLoop();

	return 0;
}