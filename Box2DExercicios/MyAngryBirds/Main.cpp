#include <cstdio>
#include <iostream>
#include <vector>
#include <GL/glut.h>
#include "Render.h"
#include "ForceFunctions.h"
#include "WorldObjects.h"


using namespace std;


struct Laucher {
	Laucher(){
		x = -80.0f;
		y = -35.0f;
		angle = 45.0;
	}
	float32 x;
	float32 y;
	float32 angle;
};

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
MyContactListener contactListener;
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


void Phase1(){

	screenGameOver = false;
	screenWinner = false;
	deadPigsCount = 0;
	maxBirds = 1;
	score = 0;

	Create4Walls(world, 87.0, 39.5f);

	// 10 Pigs
	float xi = 0.0;
	float yi = -36.0;
	for (int i=0; i < 10;i++)
	{
		pigs.push_back(CreateCircle(world, 0.0,yi,3.5,2.0,0.2,0.2));
		pigsHealth.push_back(1.0); //todos os porquinhos começam saudáveis
		yi+=7.0; //2*raio do círculo, para posicioná-los corretamente
	}

	pigsToDie.resize(pigs.size());

}


void DrawHeadsUpDisplay(){

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



//Rotina de Callback de redimensionamento da janela 
void Resize(int32 w, int32 h)
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


void ResetGame(){
	world->~b2World();
	world = NULL;
	InitBox2D();
	birds.clear();
	pigs.clear();
	pigsHealth.clear();
	pigsToDie.clear();
}


// Callback de temporizador da GLUT (pra não ficar muito rápido)
void Timer(int)
{
	if (!world)
		exit(0);
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 1);
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


//Callback de desenho da GLUT, nela é chamada a rotina que chama o passo da simulação
void SimulationLoop()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Chama a rotina que chama o passo da simulação
	RunBox2D();

	DrawHeadsUpDisplay();

	if(birds.size() > 0)
		cout << "Angular Velocity: " << birds[birds.size()-1]->GetAngularVelocity() << endl;
	
	if(birds.size() >= maxBirds && pigs.size() > deadPigsCount && !birds[birds.size()-1]->IsAwake()){
		screenGameOver = true;
	}

	b2Color color;

	//Define a cor dos objetos como vermelha
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

	// Contacts between birds and pigs
	contact = world->GetContactList();
	for (int i=0; i < world->GetContactCount(); i++)
	{
		contactListener.BeginContact(contact);
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

//Rotina de callback de teclado da GLUT
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
   	case ' ':
	{
		if(birds.size() < maxBirds){
			//Criando um passarinho...
			birds.push_back(CreateCircle(world, laucher.x, laucher.y, 2.0, 1.0, 0.5, 0.5));
		
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
		Phase1();
		break;

	//Sai do programa
	case 27:
		world->~b2World();
		world = NULL;
		exit(0);
		break;

	}

}

//Rotina que converte coordenadas de tela em coordenadas do mundo
b2Vec2 ConvertScreenToWorld(int32 x, int32 y)
{
	float32 u = x / float32(width);
	float32 v = (height - y) / float32(height);

	float32 ratio = float32(width) / float32(height);
	b2Vec2 extents(ratio * 40.0f, 40.0f); //aqui tá no braço
	//extents *= viewZoom;

	b2Vec2 lower = -extents;
	b2Vec2 upper = extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
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
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(framePeriod, Timer, 1);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	//Rotina com a inicialização do mundo
	InitBox2D();

	Phase1();

	glutMainLoop();

	return 0;
}



void MyContactListener::BeginContact(b2Contact* contact)
{
	b2Body *bodyA, *bodyB;
	bodyA = contact->GetFixtureA()->GetBody();
	bodyB = contact->GetFixtureB()->GetBody();

	char typeA = 'c'; //cenario
	char typeB = 'c'; //cenario
	
	float birdVelocity;

	for(int j=0; j < birds.size(); j++){
			
		if (bodyA == birds[j]){
			typeA = 'b'; //bird
			b2Vec2 v = birds[j]->GetLinearVelocity();
			birdVelocity = sqrt(v.x*v.x+v.y*v.y);
		}

		if (bodyB == birds[j]){
			typeB = 'b'; //bird
			b2Vec2 v = birds[j]->GetLinearVelocity();
			birdVelocity = sqrt(v.x*v.x + v.y*v.y);
		}
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

		pigsHealth[pigIndex] -= 0.025;

		if (pigsHealth[pigIndex] < 0.0){
			//acabou a saude, mata porco
			pigsToDie.push_back(pigIndex);
			score += 5000;
		}
	}
}