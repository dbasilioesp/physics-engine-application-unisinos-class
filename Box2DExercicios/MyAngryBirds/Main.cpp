#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <GL/glut.h>
#include "Render.h"
#include "ForceFunctions.h"
#include "WorldObjects.h"
#include "Laucher.h"


using namespace std;


class MyContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact){ /* handle end event */ }
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold){ /* handle pre-solve event */ }
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse){ /* handle post-solve event */ }
};


int32 mainWindow;
b2World *world;
int height=320, width=700;
int32 framePeriod = 16; // milliseconds
float32 timeStep;
int32 velocityIterations;
int32 positionIterations;
DebugDraw renderer;
Laucher laucher;
MyContactListener listener;
vector <b2Body *> birds;
vector <b2Body *> pigs;
vector <float> pigsHealth;
vector <int> pigsToDie;
bool screenGameOver;
bool screenWinner;
int deadPigsCount;
int maxBirds;
int score;
int highscore;


void Stage01(){

	screenGameOver = false;
	screenWinner = false;
	deadPigsCount = 0;
	maxBirds = 1;
	score = 0;

	Create4Walls(world, 87.0, 39.5f);
	
	b2Body *pig = CreateCircle(world, 25.0, -39.0, 3.5, 2.0, 0.2, 0.2);
	
	BodyUserData *userData = new BodyUserData();
	userData->entityType = "pig";
	userData->health = 1.0;
	pig->SetUserData((void*)userData);

	pigs.push_back(pig);
	
	pigsHealth.push_back(1.0);

	CreateWoodBarTall(world, 0, -29);

}

void DrawHud(){

	//Define a cor do texto como preta
	b2Color color; color.r = 0.0; color.g = 0.0; color.b = 0.0;

	//Imprimindo dados da simulação
	int32 bodyCount = world->GetBodyCount();
	int32 contactCount = world->GetContactCount();
	int32 jointCount = world->GetJointCount();
	renderer.DrawString(5, 15, color,"bodies/contacts/joints= %d/%d/%d", bodyCount, contactCount, jointCount);
	renderer.DrawString(5, 30, color,"birds/pigs = %d/%d", birds.size(), pigs.size()-deadPigsCount);

	if(screenGameOver){
		renderer.DrawString((width/2)-35, height/2, color, "GAME OVER");
		renderer.DrawString((width/2)-65, (height/2)+20, color, "Press R to restart");
	}

	if(screenWinner){
		renderer.DrawString((width/2)-35, height/2, color, "YOU WIN!");
		renderer.DrawString((width/2)-65, (height/2)+20, color, "Press R to restart");
	}

	renderer.DrawString(width-160, 25, color, "High Score: %d", highscore);
	renderer.DrawString(width-120, 45, color, "Score: %d", score);
}

void DrawGuideLine()
{
	glColor3f(0, 0, 1);
	b2Vec2 pInicial(laucher.x, laucher.y);
	b2Vec2 pFinal = CalculaComponentesDoVetor(10.0, laucher.angle);

	glLineWidth(3);
	glBegin(GL_LINES);
		glVertex2d(pInicial.x, pInicial.y);
		glVertex2d(pInicial.x+pFinal.x, pInicial.y+pFinal.y);
	glEnd();
	glLineWidth(1);
}

void StartBox2D()
{
	// Define the gravity vector.
	b2Vec2 gravity(0.0f, -9.8f);

	// Inicializa a biblioteca Box2D
	world = new b2World(gravity);

	world->SetContactListener(&listener);

	// Define os parâmetro para a simulação
	// Quanto maior, mais preciso, porém, mais lento
    velocityIterations = 8;
    positionIterations = 3;
	timeStep = 1.0f / 60.0f;
		
}

void ReshapeFunc(int32 w, int32 h)
{
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

void ResetGame(){
	world->~b2World();
	world = NULL;
	StartBox2D();
	birds.clear();
	pigs.clear();
	pigsHealth.clear();
	pigsToDie.clear();
}

void Timer(int)
{
	if (!world)
		exit(0);
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 1);
}

void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	world->Step(timeStep, velocityIterations, positionIterations);
	world->ClearForces();

	DrawHud();

	if(birds.size() >= maxBirds && pigs.size() > deadPigsCount && !birds[birds.size()-1]->IsAwake()){
		screenGameOver = true;
	}

	b2Color color;

	color.r = 1.0; color.g = 0.0; color.b = 0.0;
	b2Body *b;
	for(b = world->GetBodyList(); b; b=b->GetNext()){
		renderer.DrawFixture(b->GetFixtureList(),color);
	}

	// Draw birds
	color.r = 0.0; color.g = 1.0; color.b = 1.0;	
	for (int i=0; i < birds.size(); i++) 
		renderer.DrawFixture(birds[i]->GetFixtureList(), color);

	// Draw pigs
	color.r = 0.0; color.g = 0.8; color.b = 0.0;
	for (int i=0; i < pigs.size(); i++) {
		if (pigs[i] != NULL)
			renderer.DrawFixture(pigs[i]->GetFixtureList(),color);
	}

	DrawGuideLine();
	
	// Draw Contact Points
	color.r = 1.0; color.g = 0.0; color.b = 1.0;
	b2Contact * contact = world->GetContactList();
	for (int i=0; i < world->GetContactCount(); i++)
	{
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		b2Vec2 p = worldManifold.points[0];
		renderer.DrawPoint(p,4,color);
		
		contact = contact->GetNext();
	}

	// Destroy dead pigs
	for(int i=0; i < pigsToDie.size(); i++)
	{
		int pigIndex = pigsToDie[i];
		if (pigs[pigIndex] != NULL)
		{
			world->DestroyBody(pigs[pigIndex]);
			pigs[pigIndex] = NULL;
			deadPigsCount++;
		}
	}
	
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
   	case ' ':
	{
		if(birds.size() < maxBirds){
			//Criando um passarinho...

			BodyUserData *userData = new BodyUserData();
			userData->entityType = "bird";

			birds.push_back(CreateCircle(world, laucher.x, laucher.y, 2.0, 1.0, 0.5, 0.5, userData));
		
			//Aplicando uma força inicial nele...
			b2Vec2 force;
			force = CalculaComponentesDoVetor(2500, laucher.angle);
		
			birds[birds.size()-1]->ApplyForceToCenter(force, true);
		}

		break;
	}
	case '+':
	case 'a':
		laucher.angle += 5;
		break;

	case '-':
	case 's':
		laucher.angle -= 5;
		cout << laucher.angle << "\n";
		break;

	case 'r':
		ResetGame();
		Stage01();
		break;

	//Sai do programa
	case 27:
		world->~b2World();
		world = NULL;
		exit(0);
		break;

	}

}

int main(int argc, char** argv)
{
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	mainWindow = glutCreateWindow(title);
	glutDisplayFunc(SimulationLoop);
	glutReshapeFunc(ReshapeFunc);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(framePeriod, Timer, 1);

	StartBox2D();
	Stage01();

	glutMainLoop();

	return 0;
}


void MyContactListener::BeginContact(b2Contact* contact)
{
	b2Body *pigA = NULL, 
		   *pigB = NULL;
	b2Body *birdA = NULL,
		   *birdB = NULL;
	b2Body *scenaryObjectA = NULL,
		   *scenaryObjectB = NULL;
	b2Body *bodyA, *bodyB;

	bodyA = contact->GetFixtureA()->GetBody();
	bodyB = contact->GetFixtureB()->GetBody();

	BodyUserData* userDataA = (BodyUserData*)bodyA->GetUserData();
	BodyUserData* userDataB = (BodyUserData*)bodyB->GetUserData();

	if(userDataA->entityType == "bird") birdB = bodyA;
	if(userDataB->entityType == "bird") birdB = bodyB;
	if(userDataA->entityType == "pig")  pigA = bodyA;
	if(userDataB->entityType == "pig")  pigA = bodyB;
	
	char typeA = 'c'; //cenario
	char typeB = 'c'; //cenario
	
	float birdVelocity;

	if (birdA != NULL){
		b2Vec2 v = birdA->GetLinearVelocity();
		birdVelocity = sqrt(v.x*v.x+v.y*v.y);
	}

	if (birdB != NULL){
		b2Vec2 v = birdB->GetLinearVelocity();
		birdVelocity = sqrt(v.x*v.x+v.y*v.y);
	}

	int pigIndex = -1;

	for(int j=0; j < pigs.size(); j++)
	{
		if (pigs[j]) {
			if (bodyA == pigs[j]) {
				typeA = 'p'; //pig
				pigIndex = j;
			}
			if (bodyB == pigs[j]) {
				typeB = 'p'; //pig
				pigIndex = j;
			}
		}
	}

	// Provoca dano no porco
	if( (typeA == 'b'  && typeB == 'p') || (typeB == 'b'  && typeA == 'p')){

		pigsHealth[pigIndex] -= 0.25;

		if (pigsHealth[pigIndex] < 0.0){
			//acabou a saude, mata porco
			pigsToDie.push_back(pigIndex);
			score += 5000;
		}
	}
}