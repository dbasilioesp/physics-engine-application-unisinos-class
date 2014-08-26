
#include "Render.h"
#include <GL/glut.h>

#include <cstdio>
#include <iostream>

//Algumas globais para interface e simula��o
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

// O objeto World serve para armazenar os dados da simula��o --> MUNDO F�SICO DA BOX2D
b2World *world;

//Alguns corpos r�gidos
b2Body *box1, *box2, *circle1, *circle2, *line1, *line2;
b2Body* ground;

//Objeto para a classe que faz o desenho das formas de colis�o dos corpos r�gidos
DebugDraw renderer;


//Resize callback -- GLUT
void Resize(int32 w, int32 h)
{
	GLsizei largura, altura;
	
	// Evita a divisao por zero
	if(h == 0) h = 1;

	// Atualiza as vari�veis
	largura = w;
	altura = h;

	// Especifica as dimens�es da Viewport
	glViewport(0, 0, largura, altura);

	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Estabelece a janela de sele��o (esquerda, direita, inferior, 
	// superior) mantendo a propor��o com a janela de visualiza��o
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


//Fun��o de inicializa��o da Box2D
void InitBox2D()
{

	// Define the gravity vector.
	b2Vec2 gravity(9.8f, 0.0f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);
	
	// Define os par�metro para a simula��o
	// Quanto maior, mais preciso, por�m, mais lento
    velocityIterations = 6;
    positionIterations = 2;
	timeStep = 1.0f / 60.0f; //60 Hz

}


// Fun��o de Execu��o da Simula��o
void RunBox2D()
{
	world->Step(timeStep, velocityIterations, positionIterations);
	world->ClearForces();
}


// Fun��o que imprime todos os objetos  
void PrintBodies()
{
	b2Body *b;
	float ang;
	b2Vec2 pos;
	//PERCORRE A LISTA DE CORPOS R�GIDOS DO MUNDO
	for(b = world->GetBodyList(); b; b=b->GetNext())
	{
		pos = b->GetPosition();
		ang = b->GetAngle();
		printf("%4.2f %4.2f %4.2f\n", pos.x, pos.y, ang);	
	}
}

//Fun��o que chama as rotinas de desenho para cada fixture de um corpo r�gido, de acordo com o tipo da forma de colis�o dela e 
void DrawFixture(b2Fixture* fixture, b2Color color)
{
		
		const b2Transform& xf = fixture->GetBody()->GetTransform();

		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

				b2Vec2 center = b2Mul(xf, circle->m_p);
				float32 radius = circle->m_radius;

				renderer.DrawCircle(center, radius, color);
			}
			break;

		case b2Shape::e_polygon:
			{
				b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = poly->m_count;
				b2Assert(vertexCount <= b2_maxPolygonVertices);
				b2Vec2 vertices[b2_maxPolygonVertices];

				for (int32 i = 0; i < vertexCount; ++i)
				{
					vertices[i] = b2Mul(xf, poly->m_vertices[i]);
				}

				renderer.DrawPolygon(vertices, vertexCount, color);
			}
		
			break;
		case b2Shape::e_edge:
			{
				b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
				int32 vertexCount;
				
				b2Vec2 vertices[b2_maxPolygonVertices];
				int i=0;

				if (edge->m_hasVertex0) 
				{
						vertices[i] = b2Mul(xf, edge->m_vertex0);
						i++;
				}
				vertices[i] = b2Mul(xf, edge->m_vertex1); i++;
				vertices[i] = b2Mul(xf, edge->m_vertex2); i++;
				if (edge->m_hasVertex3) 
				{
						vertices[i] = b2Mul(xf, edge->m_vertex3);
						i++;
				}
				
				vertexCount = i;
				renderer.DrawPolygon(vertices, vertexCount, color);
			}
		
			break;
			
		}
	
	}

//Calback de rendering, que tamb�m chama o passo da simula��o
void SimulationLoop()
{
	b2Vec2 gravity = world->GetGravity();
	if(gravity.y > -9.81)
		gravity.y -= 0.1;
	world->SetGravity(gravity);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RunBox2D();
	PrintBodies();

	b2Body *b;
	glColor3f(1,0,0);
	glPointSize(5);

	b2Color color; color.r = 1.0; color.g = 0.0; color.b = 0.0;

	//PERCORRE A LISTA DE CORPOS R�GIDOS DO MUNDO E CHAMA A ROTINA DE DESENHO PARA A LISTA DE FIXTURES DE CADA UM
	for(b = world->GetBodyList(); b; b=b->GetNext())
	{
		DrawFixture(b->GetFixtureList(),color);
	}
		
	glutSwapBuffers();

	
}

//Callback de teckado
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	}
}


void Pause(int)
{
	//settings.pause = !settings.pause;
}


b2Body *CreateBox(float posX, float posY, 
				  float32 widthRay=5.0, float32 heightRay=5.0, 
				  float32 density=10.0, float32 friction=0.5, float32 restitution=0.5)
{
	b2Body *box;

	//Primeiro, criamos a defini��o do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_dynamicBody;

	//Estamos usando uma forma de poligono, que pode ter at� 8 v�rtices
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

b2Body *CreateEdge(float posX, float posY, const b2Vec2 &v1, const b2Vec2 &v2, 
					 float32 density=10.0, float32 friction=0.5, float32 restitution=0.5)
{
	b2Body *line;

	//Primeiro, criamos a defini��o do corpo
	b2BodyDef bodyDef;
	bodyDef.position.Set(posX, posY);
	bodyDef.type = b2_staticBody;

	//Estamos usando uma forma de poligono, que pode ter at� 8 v�rtices
	b2EdgeShape shape;
	shape.Set(v1, v2);
	
	//Por fim, criamos o corpo...
	line =  world->CreateBody(&bodyDef);

	//... e criamos a fixture do corpo 	
	line->CreateFixture(&shape, 1.0);

	return line;
}




int main(int argc, char** argv)
{
	
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

	box1 = CreateBox(-10, 15);
	
	CreateEdge(0, -38, b2Vec2(-38.5, 0.0), b2Vec2(38.5, 0.0));
	CreateEdge(0, 38, b2Vec2(-38.5, 0.0), b2Vec2(38.5, 0.0));
	CreateEdge(38, 0, b2Vec2(0, -38.5), b2Vec2(0, 38.5));
	CreateEdge(-38, 0, b2Vec2(0, -38.5), b2Vec2(0, 38.5));

	glutMainLoop();

	return 0;
}