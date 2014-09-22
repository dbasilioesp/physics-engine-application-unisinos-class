#include <cstdio>
#include <iostream>
#include <vector>
#include <GL/glut.h>
#include "Render.h"
#include "RotinasAuxiliares.h"
#include "ImagemPNM.h"
#include "Texture.cpp"


using namespace std;


int32 mainWindow;
int height=450, width=450;
b2World *world;
int32 framePeriod = 16; // milliseconds
float32 timeStep;
int32 velocityIterations;
int32 positionIterations;
DebugDraw renderer;
b2Vec2 mouseWorld;
b2MouseJoint* mouseJoint;
vector <Texture *> textures;
vector <b2Body *> birds;
vector <b2Body *> pigs;
vector <float> pigsHealth;
vector <int> pigsToDieIndexes;
int deadPigsCount = 0;
float angle = 45;
b2Body *ground;


class MyContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact)
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
				pigsToDieIndexes.push_back(pigIndex);
			}
		}

	}
	void EndContact(b2Contact* contact){ /* handle end event */ }
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold){ /* handle pre-solve event */ }
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse){ /* handle post-solve event */ }
};

MyContactListener contactListener;


//Rotina para a criação de um objeto caixa parametrizado
b2Body *CreateBox(float posX, float posY, float altura, float largura, float massa, float coefatrito, float coefrestituicao)
{
	//Cria o novo objeto (objeto novoObjeto)
	b2Body *objeto;

	//1º passo: criação da definição do corpo (b2BodyDef)
	b2BodyDef b;
	b.position.Set(posX,posY);
	b.type = b2_dynamicBody;
	//b.awake = false;
	//2º passo: criação do corpo pelo mundo (mundo cria corpo)
	objeto = world->CreateBody(&b);
	//3º passo: criação da definição da forma (b2PolygonShape, b2CircleShape ou b2EdgeShape)
	b2PolygonShape caixa;
	caixa.SetAsBox(largura/2, altura/2);
	//4º passo: criação da definição da fixture (b2FixtureDef)
		//Não esquecer de associar a forma com a fixture!
	b2FixtureDef f;
	f.shape = &caixa;
	float area = altura*largura;
	f.density = massa/area;
	f.friction = coefatrito;
	f.restitution = coefrestituicao;
	//5º passo: criação da fixture pelo corpo (objeto cria fixture)
	objeto->CreateFixture(&f);
	
	return objeto;
}

//Rotina para a criação de um objeto circulo parametrizado
b2Body *CreateCircle(float posX, float posY, float raio, float massa, float coefatrito, float coefrestituicao)
{
	//Cria o novo objeto (objeto novoObjeto)
	b2Body *objeto;

	//1º passo: criação da definição do corpo (b2BodyDef)
	b2BodyDef b;
	b.position.Set(posX,posY);
	b.type = b2_dynamicBody;
	//b.awake = false;
	//2º passo: criação do corpo pelo mundo (mundo cria corpo)
	objeto = world->CreateBody(&b);
	//3º passo: criação da definição da forma (b2PolygonShape, b2CircleShape ou b2EdgeShape)
	b2CircleShape circulo;
	circulo.m_radius = raio;

	//4º passo: criação da definição da fixture (b2FixtureDef)
		//Não esquecer de associar a forma com a fixture!
	b2FixtureDef f;
	f.shape = &circulo;
	float area = b2_pi * raio * raio;
	f.density = massa/area;
	f.friction = coefatrito;
	f.restitution = coefrestituicao;
	//5º passo: criação da fixture pelo corpo (objeto cria fixture)
	objeto->CreateFixture(&f);
	
	return objeto;
}

//Rotina que habilita uma textura na OpenGL
void EnableTextureInOpenGL(Texture *texture)
{
	// Associa a textura aos comandos seguintes
	glBindTexture(GL_TEXTURE_2D, texture->id);
	// Envia a textura para OpenGL, usando o formato RGB
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels);
	// Ajusta os filtros iniciais para a textura
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//Rotina para desenhar um sprite
void DrawSprite(float size, float posX, float posY, float posZ, float angle)
{
	//habilita transparencia
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	int final = 1.0;
	glPushMatrix();
	glTranslatef(posX,posY,posZ);
	glRotatef(angle,0,0,1);
	float radius = size/2;
	glEnable(GL_TEXTURE_2D);
	glNormal3f(0,0,1);
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex3f(-1*radius,-1*radius,0);
		glTexCoord2f(final,0);
		glVertex3f( 1*radius,-1*radius,0);
		glTexCoord2f(final,final);
		glVertex3f( 1*radius, 1*radius,0);
		glTexCoord2f(0,final);
		glVertex3f(-1*radius, 1*radius,0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glPopMatrix();
}

//Rotina que cria uma textura a partir da leitura de uma imagem PPM em modo texto
Texture *CreateTexture(char *filename)
{
	Texture *texture;
		// Carrega a textura e envia para OpenGL
	ImagemPNM image;
	image.LePNM(filename);

	texture = new(Texture);
	texture->width = image.RetornaLargura();
	texture->height = image.RetornaAltura();
	texture->pixels = image.RetornaImagem()->getPixels();
	// Gera uma identificação para a nova textura
	glGenTextures(1, &texture->id);
	
	return texture;
}

// Função responsável por inicializar parâmetros e variáveis
void InicializaTexturizacao (void)
{   
	
	//habilita transparencia
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Informa o alinhamento da textura na memória
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	textures.push_back(CreateTexture("./textures/pig.ppm"));
	textures.push_back(CreateTexture("./textures/angry.ppm"));
	textures.push_back(CreateTexture("./textures/ceu.ppm"));

	EnableTextureInOpenGL(textures[0]); //habilita a primeira textura (porquinho)

	// Seleciona o modo de aplicação da textura
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Iluminação
	// Define parâmetros de iluminação
	GLfloat luzAmb[4] = { 0.3, 0.3, 0.3, 1 };	// luz ambiente
	GLfloat luzDif[4] = { 1.0, 1.0, 1.0, 1 };	// luz difusa
	glLightfv( GL_LIGHT0, GL_AMBIENT,  luzAmb ); 
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  luzDif );

}

void DesenhaLinhaGuia()
{
		glColor3f(0,0,1);
		b2Vec2 pInicial(-35,-35);
		b2Vec2 pFinal = CalculaComponentesDoVetor(10.0,angle);

		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2d(pInicial.x,pInicial.y);
		glVertex2d(pInicial.x+pFinal.x,pInicial.y+pFinal.y);
		glEnd();
		glLineWidth(1);
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

	//Tire a gravidade! ;)
	//b2Vec2 gravity(0.0f, 0.0f);

	// Permite que os objetos 'durmam'. Melhora o desempenho
	bool doSleep = true;

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

//Rotina que chama os métodos de desenho da classe DebugDraw para desenhar os objetos da cena
void DrawShape(b2Fixture* fixture, b2Color color)
	{
		
		const b2Transform& xf = fixture->GetBody()->GetTransform();
	
		switch (fixture->GetType())
		{
		case b2Shape::e_circle:
			{
				b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

				b2Vec2 center = b2Mul(xf, circle->m_p);
				float32 radius = circle->m_radius;

				b2Vec2 axis = b2Mul(xf.q, b2Vec2(1.0f, 0.0f));

				float angle = xf.q.GetAngle();

				renderer.DrawSolidCircle(center, radius, axis, color);
				
				
				DrawSprite((radius*1.44)*2,center.x,center.y,0.0,RadianosParaGraus(angle)); //este 1.44 foi um cálculo q eu fiz para encaixar... o ideal é que o circulo tenha o raio da imagem
				
				
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
			case b2Shape::e_chain:
		{
			b2ChainShape* chain = (b2ChainShape*)fixture->GetShape();
			int32 count = chain->m_count;
			const b2Vec2* vertices = chain->m_vertices;

			b2Vec2 v1 = b2Mul(xf, vertices[0]);
			for (int32 i = 1; i < count; ++i)
			{
				b2Vec2 v2 = b2Mul(xf, vertices[i]);
				renderer.DrawSegment(v1, v2, color);
				renderer.DrawCircle(v1, 0.05f, color);
				v1 = v2;
			}
		}
		break;
			
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

// Callback de temporizador da GLUT (pra não ficar muito rápido)
void Timer(int)
{
	if (!world)
		exit(0);
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(framePeriod, Timer, 1);
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

	//Define a cor do texto como preta
	b2Color color; color.r = 0.0; color.g = 0.0; color.b = 0.0;

	//Imprimindo dados da simulação
	int32 bodyCount = world->GetBodyCount();
	int32 contactCount = world->GetContactCount();
	int32 jointCount = world->GetJointCount();
	renderer.DrawString(5, 15, color,"corpos/contatos/juntas = %d/%d/%d", bodyCount, contactCount, jointCount);
	renderer.DrawString(5, 30, color,"birds/pigs = %d/%d", birds.size(), pigs.size()-deadPigsCount);
	
	//Define a cor dos objetos como vermelha
	color.r = 1.0; color.g = 0.0; color.b = 0.0;
	
	b2Body *b;
	for(b = world->GetBodyList(); b; b=b->GetNext()){
		renderer.DrawFixture(b->GetFixtureList(),color);
	}

	//Desenha o fundo
	EnableTextureInOpenGL(textures[2]); //a textura do fundo está na posição 2
	DrawSprite(80,0,0,0,0);

	//Desenha os passarinhos
	color.r = 0.0; color.g = 1.0; color.b = 1.0;
	EnableTextureInOpenGL(textures[1]); //a textura do passaro está na posição 1
	for (int i=0; i<birds.size(); i++) 
		DrawShape(birds[i]->GetFixtureList(),color);

	//Desenha os pigs
	color.r = 0.0; color.g = 0.8; color.b = 0.0;
	EnableTextureInOpenGL(textures[0]); //a textura do passaro está na posição 0
	for (int i=0; i<pigs.size(); i++) {
		if (pigs[i] != NULL)
			DrawShape(pigs[i]->GetFixtureList(),color);
	}

	//Desenhando o ponto de picking do mouse
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex2f(mouseWorld.x,mouseWorld.y);
	glEnd();
	glPointSize(1);

	DesenhaLinhaGuia();
	
	//Para desenhar os pontos de contatos
	color.r = 1.0; color.g = 0.0; color.b = 1.0;
	b2Contact * contact = world->GetContactList();
	for (int i=0; i<world->GetContactCount(); i++)
	{
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		b2Vec2 p = worldManifold.points[0];
		renderer.DrawPoint(p,4,color);
		
		contact = contact->GetNext();
	}

	//cout << "Desenhou os contatos \n";
	//system("pause");
	//Verifica a colisão entre pássaros e pigs
	contact = world->GetContactList();
	for (int i=0; i<world->GetContactCount(); i++)
	{
		contactListener.BeginContact(contact);
		contact = contact->GetNext();
	}

	//Depois de percorrer a lista de contatos, destruir os corpos dos pigs mortos
	for(int i=0; i < pigsToDieIndexes.size(); i++)
	{
		int pigIndex = pigsToDieIndexes[i];
		if (pigs[pigIndex] != NULL) //desse jeito que eu fiz pode acontecer isso
		{
			world->DestroyBody(pigs[pigIndex]);
			pigs[pigIndex] = NULL;
			deadPigsCount++;
		}
	}
	
	glutSwapBuffers();
	//O glutPostRedisplay() está sendo dada na rotina de callback de timer!!
	
}

//Rotina de callback de teclado da GLUT
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
   	case ' ':
		{

		//Criando um passarinho...
		birds.push_back(CreateCircle(-35,-35.0,2.0,1.0,0.5,0.5));
		
		//Aplicando uma força inicial nele...
		b2Vec2 vetorForca;
		vetorForca = CalculaComponentesDoVetor(2500, angle);
		
		birds[birds.size()-1]->ApplyForceToCenter(vetorForca,true);

		break;
		}
	
	case '+':
		angle += 5;
		break;

	case '-':
		angle -= 5;
		cout << angle << "\n";
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

//Rotina que faz o picking do objeto
void MouseDown(const b2Vec2& p)
{
	mouseWorld = p;
	
	if (mouseJoint != NULL)
	{
		return;
	}

	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	world->QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();
		b2MouseJointDef md;
		md.bodyA = ground;
		md.bodyB = body;
		md.target = p;
		md.maxForce = 1000.0f * body->GetMass();
		md.dampingRatio = 1.0;
		md.collideConnected = true;
		mouseJoint = (b2MouseJoint*)world->CreateJoint(&md);
		body->SetAwake(true);
	}
}

//Rotina de callback de eventos do mouse
void Mouse(int32 button, int32 state, int32 x, int32 y)
{
	// Use the mouse to move things around.
	if (button == GLUT_LEFT_BUTTON)
	{
		int mod = glutGetModifiers();
		b2Vec2 p = ConvertScreenToWorld(x, y);

		if (state == GLUT_DOWN)
		{
			MouseDown(p);
		}	
		if (state == GLUT_UP)
		{
			if(mouseJoint)
				world->DestroyJoint(mouseJoint);
			mouseJoint = NULL;
		}	
	}

}

//Para atualizar a junta de mouse quando o mouse se move
void MouseMove(const b2Vec2& p)
{
	mouseWorld = p;
	
	if (mouseJoint)
	{
		mouseJoint->SetTarget(p);
	}

}

//Rotina de callback para movimentação do mouse
void MouseMotion(int32 x, int32 y)
{
	b2Vec2 p = ConvertScreenToWorld(x, y);
	MouseMove(p);
}

//Main :)
int main(int argc, char** argv)
{
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d -- Aprendendo a usar contatos", b2_version.major, b2_version.minor, b2_version.revision);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	mainWindow = glutCreateWindow(title);
	glutDisplayFunc(SimulationLoop);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(framePeriod, Timer, 1);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	// Rotina com a inicialização do mundo
	InitBox2D();

	// Cria o chão
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	ground->CreateFixture(&shape,0.0);

	// Cria o teto
	b2Body* teto = world->CreateBody(&bd);
	shape.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
	teto->CreateFixture(&shape,0.0);

	// Cria a parede esquerda
	b2Body* paredeEsquerda = world->CreateBody(&bd);
	shape.Set(b2Vec2(-39.5, 39.5), b2Vec2(-39.5, -39.5));
	paredeEsquerda->CreateFixture(&shape,0.0);

	// Cria a parede direita
	b2Body* paredeDireita = world->CreateBody(&bd);
	shape.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
	paredeDireita->CreateFixture(&shape,0.0);
	
	// Criando 10 porquinhos
	float xi = 0.0;
	float yi = -36.0;
	for (int i=0; i<10;i++)
	{
		pigs.push_back(CreateCircle(0.0,yi,3.5,2.0,0.2,0.2));
		pigsHealth.push_back(1.0); //todos os porquinhos começam saudáveis
		yi+=7.0; //2*raio do círculo, para posicioná-los corretamente
	}
	pigsToDieIndexes.resize(pigs.size());

	//Para ler as imagens dos sprites...
	InicializaTexturizacao();

	glutMainLoop();

	return 0;
}
