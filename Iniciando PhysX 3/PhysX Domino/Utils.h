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


using namespace physx;
using namespace std;


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


void SetOrthoForFont(int windowWidth, int windowHeight)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, 0, windowHeight);
	glScalef(1, -1, 1);
	glTranslatef(0, -windowHeight, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void ResetPerspectiveProjection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}


/************************************************************************/
/* Draws                                                                     */
/************************************************************************/

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
	glScalef(cubeGeometry.halfExtents.x*2, cubeGeometry.halfExtents.y*2, cubeGeometry.halfExtents.z*2);
	glutSolidCube(1);
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