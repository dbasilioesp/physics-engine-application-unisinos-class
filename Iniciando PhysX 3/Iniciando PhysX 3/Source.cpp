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
#include <extensions\PxSimpleFactory.h>

#include <foundation\PxFoundation.h>
#include <foundation\PxMat33.h>

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
int state = 1;
int oldX = 0;
int oldY = 0;
float rX = 0;
float rY = 50;
float dist = 0;
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
GLfloat lightAmbientColour[] = {0.4f, 0.4f, 0.4f, 1.0f};
GLfloat lightDiffuseColour[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat lightSpecularColour[] = {0.8f, 0.8f, 0.8f, 1.0f};


void InitializePhysX();
void StepPhysX();
void OnRender();
void RenderActors();
void OnReshape(int newWidth, int newHeight);
void OnIdle();
void Mouse(int button, int s, int x, int y);
void Motion(int x, int y);
void OnShutdown();
void ShutdownPhysX();
void SetOrthoForFont();
void ResetPerspectiveProjection();
void RenderSpacedBitmapString(int x, int y, int spacing, void *font, char *string);
void DrawAxes();
void DrawGrid(const float gridSize);
void DrawPlane();
void DrawBox(PxShape *shape, PxRigidActor *actor);
void DrawSphere(PxShape *shape, PxRigidActor *actor);
void DrawShape(PxShape *shape, PxRigidActor *actor);
void DrawActor(PxRigidActor *actor);
void getColumnMajor(PxMat33 rowMatriz, PxVec3 t, float *columnMatriz);


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

	PxMaterial *planeMaterial = gPhysicsSDK->createMaterial(0.9f, 0.1f, 1.0f);
	PxMaterial *cubeMaterial = gPhysicsSDK->createMaterial(0.1f, 0.4f, 1.0f);
	PxMaterial *sphereMaterial = gPhysicsSDK->createMaterial(0.6f, 0.1f, 0.6f);

	PxTransform pose = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), 
								   PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic *plane = gPhysicsSDK->createRigidStatic(pose);
	PxShape *shape = plane->createShape(PxPlaneGeometry(), *planeMaterial);
	gScene->addActor(*plane);
	sceneObjects.push_back(plane);

	/*PxTransform leftPose = PxTransform(PxVec3(0.0f, 0.0f, 10.0f),
									   PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));
	PxRigidStatic *leftPlane = gPhysicsSDK->createRigidStatic(leftPose);
	PxShape *leftShape = leftPlane->createShape(PxPlaneGeometry(), *planeMaterial);
	gScene->addActor(*leftPlane);

	PxTransform rightPose = PxTransform(PxVec3(-10.0f, 0.0f, 0.0f),
									   PxQuat(PxHalfPi, PxVec3(1.0f, 0.0f, 0.0f)));
	PxRigidStatic *rightPlane = gPhysicsSDK->createRigidStatic(rightPose);
	PxShape *rightShape = rightPlane->createShape(PxPlaneGeometry(), *planeMaterial);
	gScene->addActor(*rightPlane);

	PxTransform leftPose2 = PxTransform(PxVec3(0.0f, 0.0f, -10.0f),
									   PxQuat(PxHalfPi, PxVec3(0.0f, -1.0f, 0.0f)));
	PxRigidStatic *leftPlane2 = gPhysicsSDK->createRigidStatic(leftPose2);
	PxShape *leftShape2 = leftPlane2->createShape(PxPlaneGeometry(), *planeMaterial);
	gScene->addActor(*leftPlane2);

	PxTransform rightPose2 = PxTransform(PxVec3(10.0f, 0.0f, 0.0f),
									     PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
	PxRigidStatic *rightPlane2 = gPhysicsSDK->createRigidStatic(rightPose2);
	PxShape *rightShape2 = rightPlane2->createShape(PxPlaneGeometry(), *planeMaterial);
	gScene->addActor(*rightPlane2);*/

	// Cube Actor
	PxReal cubeDensity = 2.0f;
	PxTransform cubeTransform(PxVec3(0.0f, 0.5, 5.0f));
	PxBoxGeometry cubeGeometry(PxVec3(0.5, 0.5, 0.5));

	PxRigidDynamic *cubeActor = PxCreateDynamic(*gPhysicsSDK, cubeTransform, cubeGeometry, 
												 *cubeMaterial, cubeDensity);

	cubeActor->setAngularDamping(0.0);
	cubeActor->setLinearDamping(0.0);
	cubeActor->setMass(20);

	gScene->addActor(*cubeActor);
	boxes.push_back(cubeActor);

	// Sphere Actor
	/*PxReal sphereDensity = 2.0f;
	PxTransform sphereTransform(PxVec3(0.0f, 4.0f, 0.0f));
	PxSphereGeometry sphereGeometry(0.7f);

	PxRigidDynamic *sphereActor = PxCreateDynamic(*gPhysicsSDK, sphereTransform, sphereGeometry,
												  *sphereMaterial, sphereDensity);

	sphereActor->setAngularDamping(0.2);
	sphereActor->setLinearDamping(0.01);
	sphereActor->setMass(5);

	gScene->addActor(*sphereActor);
	spheres.push_back(sphereActor);*/
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

	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	GLdouble viewer[3] = {20*sin(0.0), 20, 20*cos(0.0)};

	glTranslatef(0, 0, dist);
	gluLookAt(viewer[0], viewer[1], viewer[2], 
			  0, 0, 0, 
			  0, 1, 0);
	glRotatef(rX, 1, 0, 0);
	glRotatef(rY, 0, 1, 0);

	DrawAxes();

	DrawGrid(10);

	glColor3f(0.7f, 0.7f, 0.7f);

	glPushMatrix();
	glTranslatef(0, 10, 10);
	glRotatef(90, 1, 0, 0);
	DrawPlane();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 10, -10);
	glRotatef(-90, 1, 0, 0);
	DrawPlane();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 10, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, 10, 0);
	DrawPlane();
	glPopMatrix();

	RenderActors();

	SetOrthoForFont();
	glColor3f(1, 1, 1);
	RenderSpacedBitmapString(20, 20, 0, GLUT_BITMAP_HELVETICA_12, buffer);

	ResetPerspectiveProjection();

	glutSwapBuffers();

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

	GLfloat ambient_cube[] = {0.0f, 0.0f, 0.30f, 0.40f};
	GLfloat ambient_sphere[] = {0.20f, 0.20f, 0.0f, 0.25f};

	GLfloat diffuse_cube[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat diffuse_sphere[] = {1.0f, 1.0f, 1.0f, 1.0f};

	GLfloat matriz_diffuse_cube[] = {0.85f, 0.0f, 0.0f, 1.0f};
	GLfloat matriz_diffuse_sphere[] = {0.85f, 0.85f, 0.0f, 1.0f};
	

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matriz_diffuse_cube);
	for (int i = 0; i < boxes.size(); i++){
		DrawActor(boxes[i]);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matriz_diffuse_sphere);
	for (int i = 0; i < spheres.size(); i++){
		DrawActor(spheres[i]);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambient_cube);
	for (int i = 0; i < sceneObjects.size(); i++){
		DrawActor(sceneObjects[i]);
	}

	glDisable(GL_LIGHTING);
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
		oldX = x;
		oldY = y;
	}

	if(button == GLUT_RIGHT_BUTTON)
		state = 1;
	else
		state = 0;
}


void Motion(int x, int y)
{
	if(state == 1)
	{
		rY += (x - oldX)/5.0f;
		rX += (y - oldY)/5.0f;
	}

	oldX = x;
	oldY = y;

	glutPostRedisplay();
}


void OnIdle()
{
	glutPostRedisplay();
}


void OnShutdown()
{
	ShutdownPhysX();
}


void ShutdownPhysX()
{
	gScene->release();
	gPhysicsSDK->release();
}


void SetOrthoForFont()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	glScalef(1, -1, 1);
	glTranslatef(0, -WINDOW_HEIGHT, 0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void ResetPerspectiveProjection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}


void RenderSpacedBitmapString(int x, int y, int spacing, void *font, char *string)
{
	char *c;
	int xl = x;

	for (c = string; *c != '\0'; c++)
	{
		glRasterPos2d(xl, y);
		glutBitmapCharacter(font, *c);
		xl = xl + glutBitmapWidth(font, *c) + spacing;
	}
}


void DrawAxes()
{
	glPushMatrix();
	glColor3f(0, 0, 1);
	
	glPushMatrix();
	glTranslatef(0, 0, 0.8f);
	glutSolidCone(0.0325, 0.2, 4,1);
	glTranslatef(0, 0.0625, 0.225f);
	RenderSpacedBitmapString(0,0,0,GLUT_BITMAP_HELVETICA_10, "Z");
	glPopMatrix();

	glutSolidCone(0.0225, 1, 4,1);
	glColor3f(1, 0, 0);
	glRotatef(90, 0, 1, 0);
	glutSolidSphere(0.1, 10, 10);  // Point of reference

	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.8f);
	glutSolidCone(0.0325, 0.2, 4,1);
	glTranslatef(0.0, 0.0625, 0.225f);
	RenderSpacedBitmapString(0, 0, 0, GLUT_BITMAP_HELVETICA_10, "X");
	glPopMatrix();

	glutSolidCone(0.0225,1, 4, 1);
	glColor3f(0, 1, 0);
	glRotatef(90, -1, 0, 0);

	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.8f);
	glutSolidCone(0.0325, 0.2, 4,1);
	glTranslatef(0.0, 0.0625, 0.225f);
	RenderSpacedBitmapString(0, 0, 0, GLUT_BITMAP_HELVETICA_10, "Y");
	glPopMatrix();

	glutSolidCone(0.0225,1, 4,1);
	glPopMatrix();
}


void DrawGrid(const float gridSize)
{
	glBegin(GL_LINES);
	glColor3f(0.7f, 0.7f, 0.7f);

	for (float i = -gridSize; i <= gridSize; i++)
	{
		glVertex3f(i, 0, -gridSize);
		glVertex3f(i, 0,  gridSize);

		glVertex3f(-gridSize, 0, i);
		glVertex3f( gridSize, 0, i);
	}

	glEnd();
}


void DrawPlane()
{
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	glVertex3fv(planeVertices[0]);
	glVertex3fv(planeVertices[1]);
	glVertex3fv(planeVertices[2]);
	glVertex3fv(planeVertices[3]);
	glEnd();
}


void DrawBox(PxShape *shape, PxRigidActor *actor)
{
	PxTransform transform = PxShapeExt::getGlobalPose(*shape, *actor);
	PxBoxGeometry cubeGeometry;
	shape->getBoxGeometry(cubeGeometry);
	PxMat33 rowMatriz(transform.q);
	float columnMatriz[16];
	getColumnMajor(rowMatriz, transform.p, columnMatriz);

	glPushMatrix();
	glMultMatrixf(columnMatriz);
	glutSolidCube(cubeGeometry.halfExtents.x * 2);
	glPopMatrix();
}


void DrawSphere(PxShape *shape, PxRigidActor *actor)
{
	PxTransform transform = PxShapeExt::getGlobalPose(*shape, *actor);
	PxSphereGeometry sphereGeometry;
	shape->getSphereGeometry(sphereGeometry);
	PxMat33 rowMatriz(transform.q);
	float columnMatriz[16];
	getColumnMajor(rowMatriz, transform.p, columnMatriz);

	glPushMatrix();
	glMultMatrixf(columnMatriz);
	glutSolidSphere(sphereGeometry.radius, 20, 20);
	glPopMatrix();
}


void DrawShape(PxShape *shape, PxRigidActor *actor)
{
	PxGeometryType::Enum type = shape->getGeometryType();

	switch (type)
	{
	case physx::PxGeometryType::eSPHERE:
		DrawSphere(shape, actor);
		break;
	case physx::PxGeometryType::eBOX:
		DrawBox(shape, actor);
		break;
	default:
		break;
	}
}


void DrawActor(PxRigidActor *actor)
{
	PxU32 shapesNumber = actor->getNbShapes();

	PxShape **shapes = new PxShape*[shapesNumber];
	actor->getShapes(shapes, shapesNumber);
	while(shapesNumber--)
		DrawShape(shapes[shapesNumber], actor);

	delete [] shapes;
}


void getColumnMajor(PxMat33 rowMatriz, PxVec3 t, float *columnMatriz)
{
	columnMatriz[0] = rowMatriz.column0[0];
	columnMatriz[1] = rowMatriz.column0[1];
	columnMatriz[2] = rowMatriz.column0[2];
	columnMatriz[3] = 0;

	columnMatriz[4] = rowMatriz.column1[0];
	columnMatriz[5] = rowMatriz.column1[1];
	columnMatriz[6] = rowMatriz.column1[2];
	columnMatriz[7] = 0;

	columnMatriz[8] = rowMatriz.column2[0];
	columnMatriz[9] = rowMatriz.column2[1];
	columnMatriz[10] = rowMatriz.column2[2];
	columnMatriz[11] = 0;

	columnMatriz[12] = t[0];
	columnMatriz[13] = t[1];
	columnMatriz[14] = t[2];
	columnMatriz[15] = 1;
}