
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
#include <extensions\PxSimpleFactory.h>

#include <foundation\PxFoundation.h>

#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PxTaskDEBUG.lib")


using namespace physx;
using namespace std;

static PxPhysics* gPhysicsSDK = NULL;
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxSimulationFilterShader gDefaultFilterShader=PxDefaultSimulationFilterShader;

int startTime = 0, totalFrames = 0;
float fps = 0;
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

PxScene* gScene = NULL;
PxReal myTimestep = 1.0f/60.0f;

void StepPhysX()
{
gScene->simulate(myTimestep);

while(!gScene->fetchResults() )
{
// do something useful
}
}

void InitializePhysX()
{
PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale() );

PxInitExtensions(*gPhysicsSDK);

// Create the scene
PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

if(!sceneDesc.cpuDispatcher)
{
PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
sceneDesc.cpuDispatcher = mCpuDispatcher;
}

if(!sceneDesc.filterShader)
sceneDesc.filterShader  = gDefaultFilterShader;

gScene = gPhysicsSDK->createScene(sceneDesc);

}

void ShutdownPhysX()
{
gScene->release();
gPhysicsSDK->release();
}

void OnShutdown()
{
ShutdownPhysX();
}

void OnRender()
{
totalFrames++;
int current = glutGet(GLUT_ELAPSED_TIME);
if((current-startTime)>1000)
{
float elapsedTime = float(current-startTime);
fps = ((totalFrames * 1000.0f)/ elapsedTime);
startTime = current;
totalFrames = 0;
}

if (gScene)
StepPhysX();

glClearColor(0.1, 0.1, 0.1, 1.0);
glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
glLoadIdentity();

glutSwapBuffers();

}

void OnIdle()
{
glutPostRedisplay();
}

void main(int argc, char** argv)
{
atexit(OnShutdown);
glutInit(&argc, argv);

glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
glutCreateWindow("Initialization");

glutDisplayFunc(OnRender);
glutIdleFunc(OnIdle);

InitializePhysX();

glEnable(GL_DEPTH_TEST);
glDepthMask(GL_TRUE);
glutMainLoop();
}