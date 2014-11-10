/*
*	http://www.devkast.com/2013/03/07/create-bouncing-objects-in-a-room-with-physx-3-2-1/
*/

#define MAX_PATH 16384
#include <stdio.h>
#include <iostream>
#include <vector>
#include <GL\glut.h>

#include <PxPhysicsAPI.h>
#include <extensions\PxExtensionsAPI.h>
#include <extensions\PxDefaultErrorCallback.h>
#include <extensions\PxDefaultAllocator.h>
#include <extensions\PxDefaultSimulationFilterShader.h>
#include <extensions\PxDefaultCpuDispatcher.h>
#include <extensions\PxShapeExt.h>
#include <extensions\PxSimpleFactorY.h>

#include <foundation\PxFoundation.h>
#include <foundation\PxMat33.h>

#include "Utils.h"

#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PxTask.lib")

using namespace physx;
using namespace std;

static PxPhysics* gPhysicsSDK = NULL;
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;

typedef GLfloat point3[3];

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
PxScene *gScene = NULL;
PxReal myTimestep = 1.0f/60.0f;
int startTime = 0;
int totalFrames = 0;
float fps = 0;
char buffer[MAX_PATH];
int mouseState = 1;
int mouseOldX = 0;
int mouseOldY = 0;
float cameraRotateX = -20;
float cameraRotateY = 0;
point3 planeVertices[4] = {
	{-10,  0,  10},
	{-10,  0, -10},
	{ 10,  0, -10},
	{ 10,  0,  10}
};
const float globalGravity = -9.8;
const int objectsCount = 150;
vector<PxRigidActor*> boxes;
vector<PxRigidActor*> spheres;
vector<PxRigidStatic*> sceneObjects;
GLfloat lightAmbientColour[] = {0.9f, 0.9f, 0.9f, 1.0f};
GLfloat lightDiffuseColour[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat lightSpecularColour[] = {0.8f, 0.8f, 0.8f, 1.0f};
PxMaterial *cubeMaterial;
PxMaterial *planeMaterial;
PxMaterial *sphereMaterial;

void InitializePhysX();
void SetPhysicsMaterial();
void CreateFirstScene();
PxRigidDynamic* CreateDominoPiece(PxVec3 position);
PxRigidDynamic* CreateBall(PxVec3 position);
void StepPhysX();
void OnRender();
void RenderActors();
void OnReshape(int newWidth, int newHeight);
void OnIdle();
void Mouse(int button, int s, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void OnShutdown();


void main(int argc, char** argv)
{
	atexit(OnShutdown);
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Initialization");

	glutDisplayFunc(OnRender);
	glutIdleFunc(OnIdle);
	glutReshapeFunc(OnReshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);

	InitializePhysX();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glutMainLoop();
}


void InitializePhysX()
{
	PxFoundation *foundation = PxCreateFoundation(PX_PHYSICS_VERSION, 
												  gDefaultAllocatorCallback, 
												  gDefaultErrorCallback);

	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());

	PxInitExtensions(*gPhysicsSDK);

	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, globalGravity, 0.0f);

	if(!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher *cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = cpuDispatcher;
	}

	if(!sceneDesc.filterShader)
	{
		sceneDesc.filterShader = gDefaultFilterShader;
	}

	gScene = gPhysicsSDK->createScene(sceneDesc);

	SetPhysicsMaterial();

	CreateFirstScene();
}


void SetPhysicsMaterial(){
	planeMaterial = gPhysicsSDK->createMaterial(0.9f, 0.1f, 1.0f);
	cubeMaterial = gPhysicsSDK->createMaterial(0.1f, 0.4f, 1.0f);
	sphereMaterial = gPhysicsSDK->createMaterial(0.6f, 0.1f, 0.6f);
}


void CreateFirstScene(){

	PxTransform pose = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), 
								   PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic *plane = gPhysicsSDK->createRigidStatic(pose);
	PxShape *shape = plane->createShape(PxPlaneGeometry(), *planeMaterial);
	gScene->addActor(*plane);
	sceneObjects.push_back(plane);

	PxReal positions[] = {
	   -5.0f, 0.7, 5.0f,
	   -4.0f, 0.7, 5.0f,
	   -3.0f, 0.7, 5.0f,
	   -2.0f, 0.7, 5.0f,
	   -1.0f, 0.7, 5.0f,
		0.0f, 0.7, 5.0f,
		1.0f, 0.7, 5.0f,
		2.0f, 0.7, 5.0f,
		3.0f, 0.7, 5.0f,
		4.0f, 0.7, 5.0f,
		5.0f, 0.7, 5.0f
	};

	PxRigidDynamic *cubeActor;

	for (int i = 0; i < 11; i++)
	{
		PxVec3 position = PxVec3(positions[3*i], positions[3*i+1], positions[3*i+2]);

		cubeActor = CreateDominoPiece(position);
		gScene->addActor(*cubeActor);
		boxes.push_back(cubeActor);
	}

	// Cube Actor
	/*PxRigidDynamic *cubeActor = CreateDominoPiece(PxVec3(0.0f, 0.7, 5.0f));
	gScene->addActor(*cubeActor);
	boxes.push_back(cubeActor);

	cubeActor = CreateDominoPiece(PxVec3(1.0f, 0.7, 5.0f));
	gScene->addActor(*cubeActor);
	boxes.push_back(cubeActor);*/

	// Sphere Actor
	PxRigidDynamic *sphereActor = CreateBall(PxVec3(8.0f, 1.0f, 5.0f));
	gScene->addActor(*sphereActor);
	spheres.push_back(sphereActor);

	sphereActor->addForce(PxVec3(-800.0, 0.0, 0.0));

}


PxRigidDynamic* CreateDominoPiece(PxVec3 position){
	
	PxReal cubeDensity = 2.0f;
	PxTransform cubeTransform(position);
	PxBoxGeometry cubeGeometry(PxVec3(0.1, 0.69, 0.3));

	PxRigidDynamic *cubeActor = PxCreateDynamic(*gPhysicsSDK, cubeTransform, cubeGeometry, 
												*cubeMaterial, cubeDensity);

	cubeActor->setAngularDamping(1.0);
	cubeActor->setLinearDamping(1.0);
	cubeActor->setMass(20);

	return cubeActor;
}


PxRigidDynamic* CreateBall(PxVec3 position){

	PxReal sphereDensity = 2.0f;
	PxTransform sphereTransform(position);
	PxSphereGeometry sphereGeometry(0.5f);

	PxRigidDynamic *sphereActor = PxCreateDynamic(*gPhysicsSDK, sphereTransform, sphereGeometry,
		*sphereMaterial, sphereDensity);

	sphereActor->setAngularDamping(0.2);
	sphereActor->setLinearDamping(0.01);
	sphereActor->setMass(5);

	return sphereActor;
}


void RenderActors()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	GLfloat lightPosition[4] = {20*cos(0.0), 20*sin(0.0), 0.0, 1.0};
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmbientColour);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseColour);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecularColour);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	GLfloat mat_diffuse_cube[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat mat_diffuse_sphere[] = {0.85f, 0.85f, 0.0f, 1.0f};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse_cube);
	for (int i = 0; i < boxes.size(); i++){
		DrawActor(boxes[i]);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse_sphere);
	for (int i = 0; i < spheres.size(); i++){
		DrawActor(spheres[i]);
	}

	glDisable(GL_LIGHTING);
}


void StepPhysX()
{
	gScene->simulate(myTimestep);

	while(!gScene->fetchResults()){
		// do something useful
	}

}


void OnRender()
{

	totalFrames++;

	int current = glutGet(GLUT_ELAPSED_TIME);
	if((current-startTime) > 1000)
	{
		float elapsedTime = float(current-startTime);
		fps = ((totalFrames*1000.0f) / elapsedTime);
		startTime = current;
		totalFrames = 0;
	}

	sprintf_s(buffer, " *** ROTATE WITH MOUSE RIGHT_BUTTON | FPS: %3.2f", fps);


	if(gScene)
	{
		StepPhysX();
	}

	glClearColor(0.1, 0.2, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	GLdouble viewer[3] = {20*sin(0.0), 20, 20*cos(0.0)};

	glTranslatef(0, 0, 0);
	gluLookAt(viewer[0], viewer[1], viewer[2], 
			  0, 0, 0, 
			  0, 1, 0);
	glRotatef(cameraRotateX, 1, 0, 0);
	glRotatef(cameraRotateY, 0, 1, 0);

	DrawAxes();
	DrawGrid(10);
	RenderActors();

	SetOrthoForFont(WINDOW_WIDTH, WINDOW_HEIGHT);
	glColor3f(1, 1, 1);
	RenderSpacedBitmapString(20, 20, 0, GLUT_BITMAP_HELVETICA_12, buffer);

	ResetPerspectiveProjection();

	glutSwapBuffers();

}


void OnReshape(int newWidth, int newHeight)
{
	GLfloat ratio = (GLfloat) newWidth / (GLfloat) newHeight;

	glViewport(0, 0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, ratio, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
}


void Mouse(int button, int s, int x, int y)
{
	if(s == GLUT_DOWN)
	{
		mouseOldX = x;
		mouseOldY = y;
	}

	if(button == GLUT_RIGHT_BUTTON)
		mouseState = 1;
	else
		mouseState = 0;
}


void Motion(int x, int y)
{
	if(mouseState == 1)
	{
		cameraRotateY += (x - mouseOldX)/5.0f;
		cameraRotateX += (y - mouseOldY)/5.0f;
	}

	mouseOldX = x;
	mouseOldY = y;

	glutPostRedisplay();
}


void Keyboard(unsigned char key, int x, int y){

	switch (key)
	{
	case 27:
		exit(0);
		break;
	default:
		break;
	}

}

void OnIdle()
{
	glutPostRedisplay();
}


void OnShutdown()
{
	gScene->release();
	gPhysicsSDK->release();
}
