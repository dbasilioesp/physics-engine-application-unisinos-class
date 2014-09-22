#include "Render.h"
#include "RotinasAuxiliares.h"
#include <GL/glut.h>
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace std;


int32 framePeriod = 16; //milliseconds
int32 mainWindow;
int height=450, width=450;
float32 timeStep;
int32 velocityIterations ;
int32 positionIterations ;
b2Vec2 viewCenter(0.0f, 0.0f);
b2World *world;
b2Body* chao;
b2Body* paredeEsquerda;
b2Body* paredeDireita;
b2Body* teto;
b2Body *caixa;
b2Body *car;
b2Body *caixas[100];
int contCaixas = 0;
DebugDraw renderer;
bool desenhaLinhaGuia = false;
float angulo = 45;
// Para o picking dos objetos
b2Vec2 mouseWorld;
b2MouseJoint* mouseJoint;
// Para exemplo das juntas
b2Body *body1, *body2;
b2DistanceJoint *distanceJoint;
b2RevoluteJoint *revoluteJoint1, *revoluteJoint2;
b2PrismaticJoint *pJoint;
b2GearJoint *gearJoint;
b2PulleyJoint *pulleyJoint;
b2WheelJoint *wheelJoint1, *wheelJoint2;


//Estrutuda de dados que armazena o ponto do objeto onde a força foi aplicada e a força (vetor)
struct forcaAplicada
{
	b2Vec2 vetorForca;
	b2Vec2 pontoAplicado;
	forcaAplicada *proxima;
};

//Lista encadeada das forças aplicadas em determinado instante de simulação
forcaAplicada *ListaDeForcasAplicadas = NULL;
forcaAplicada *ultima = NULL;


//Rotina que adiciona uma força aplicada na lista
void AdicionaForcaAplicada(b2Vec2 p, b2Vec2 v)
{
	forcaAplicada *nova = new forcaAplicada();
	nova->pontoAplicado = p;
	nova->vetorForca = normaliza(v);
	nova->proxima = NULL;
	if (ListaDeForcasAplicadas == NULL) //primeira da lista
	{	
		ListaDeForcasAplicadas = nova;
		ultima = nova;
	}
	else
	{
		ultima->proxima = nova;
		ultima = nova;
	}
}

//Rotina que desenha o vetor (normalizado * 10) das forças aplicadas
void DesenhaForcasAplicadas()
{
	forcaAplicada *p;
	p = ListaDeForcasAplicadas;
	//Percorre a lista
	while (p != NULL)
	{
		glColor3f(0,0,1);
		b2Vec2 pFinal;

		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2d(p->pontoAplicado.x,p->pontoAplicado.y);
		pFinal.x =  p->pontoAplicado.x + p->vetorForca.x * 5;
		pFinal.y =  p->pontoAplicado.y + p->vetorForca.y * 5;
		glVertex2d( pFinal.x,pFinal.y);
		glEnd();
		glLineWidth(1);

		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex2d( pFinal.x,pFinal.y);
		glEnd();
		//Atualiza o ponteiro com a proxima posicao
		p = p->proxima;		
	}

	//Reseta a lista
	ListaDeForcasAplicadas = NULL;
	ultima = NULL;
}

void DesenhaLinhaGuia()
{
		glColor3f(0,0,1);
		b2Vec2 pInicial(-35,-35);
		b2Vec2 pFinal = CalculaComponentesDoVetor(10.0,angulo);

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

//Rotina que chama os métodos de desenho da classe DebugDraw para desenhar os objetos da cena
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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Chama a rotina que chama o passo da simulação
	RunBox2D();

	//Define a cor dos objetos como vermelha
	b2Color color; color.r = 1.0; color.g = 0.0; color.b = 0.0;
	
	//Desenha os objetos
	b2Body *b;
	
	for(b = world->GetBodyList(); b; b=b->GetNext())
	{
		DrawFixture(b->GetFixtureList(),color);
	}

	color.r = 1.0; color.g = 0.0; color.b = 1.0;
	if (distanceJoint)
		DrawJoint(distanceJoint,color);

	if (revoluteJoint1)
		DrawJoint(revoluteJoint1, color);
	if (revoluteJoint2)
		DrawJoint(revoluteJoint2, color);

	if (pJoint)
		DrawJoint(pJoint,color);

	if (gearJoint)
		DrawJoint(gearJoint,color);

	if (wheelJoint1)
		DrawJoint(wheelJoint1,color);
	if (wheelJoint2)
		DrawJoint(wheelJoint2,color);

	if (pulleyJoint) //Pulley joint
	{
		b2Vec2 anchor1 = pulleyJoint->GetAnchorA();
		b2Vec2 anchor2 = pulleyJoint->GetAnchorB();
		b2Vec2 ground1 = pulleyJoint->GetGroundAnchorA();
		b2Vec2 ground2 = pulleyJoint->GetGroundAnchorB();

		glBegin(GL_LINES);
		glVertex2f(anchor1.x,anchor1.y);
		glVertex2f(ground1.x,ground1.y);
		glVertex2f(anchor2.x,anchor2.y);
		glVertex2f(ground2.x,ground2.y);
		glVertex2f(ground1.x,ground1.y);
		glVertex2f(ground2.x,ground2.y);
		glEnd();

	}

	//Desenha as forças aplicadas
	DesenhaForcasAplicadas();

	//Desenhando o ponto de picking do mouse
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex2f(mouseWorld.x,mouseWorld.y);
	glEnd();
	glPointSize(1);

	if(desenhaLinhaGuia == true)
		DesenhaLinhaGuia();
	
	ostringstream aux1;  //incluir sstream
	aux1 << "Comandos: r, R, p, P, g, w, t";
	renderer.DrawString(10,20,aux1.str().c_str());
	
	glutSwapBuffers();

}

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

//Rotina de callback de teclado da GLUT
void Keyboard(unsigned char key, int x, int y)
{

	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
    case 'd': //Distance Joint
		{
			body1 = CreateBox(-5,0.0,5.0,5.0,1.0,0.5,0.5);
			body2 = CreateCircle(5,0.0,2.5,1.0,0.5,0.5);
			
			b2DistanceJointDef dJointDef;
			dJointDef.Initialize(body1, body2, body1->GetWorldCenter(), body2->GetWorldCenter());
			dJointDef.collideConnected = true;

			distanceJoint = (b2DistanceJoint*)world->CreateJoint(&dJointDef);
			break;
		}
	case 'r': //Revolute Joint
		{
			body1 = CreateBox(-20.0, 20.0,8.0,5.0,1.0,0.5,0.5);
			body2 = CreateBox(-20.0, 12.0,8.0,5.0,1.0,0.5,0.5);
			b2Vec2 worldAnchorOnBody1 = body1->GetWorldPoint(b2Vec2(0,-0.0)); 

			body1->SetType(b2_staticBody);
	
			b2RevoluteJointDef rJointDef;
			rJointDef.Initialize(body1, body2, worldAnchorOnBody1);
			rJointDef.collideConnected = false;

			//Para criar um motor
			rJointDef.maxMotorTorque = 1000.0f; //N*m
			rJointDef.motorSpeed = 100.0f; //radianos por segundo 
			rJointDef.enableMotor = true;

			revoluteJoint1 = (b2RevoluteJoint*)world->CreateJoint(&rJointDef);
			break;
		}
	case 'R': //Revolute Joint
		{
			// Second Revolute Joint
			
			body1 = CreateBox(0.0, 20.0,8.0,5.0,1.0,0.5,0.5);
			body2 = CreateBox(0.0, 12.0,8.0,5.0,1.0,0.5,0.5);
			b2Vec2 worldAnchorOnBody1 = body1->GetWorldPoint(b2Vec2(0, -4.0)); 

			body1->SetType(b2_staticBody);

			b2RevoluteJointDef rJointDef;
			rJointDef.Initialize(body1, body2,worldAnchorOnBody1);
			rJointDef.collideConnected = false;

			//Para limitar o angulo
			rJointDef.lowerAngle = GrausParaRadianos(-45); // -45 degrees
			rJointDef.upperAngle =GrausParaRadianos(45); // 45 degrees
			rJointDef.enableLimit = true;
			
			revoluteJoint2 = (b2RevoluteJoint*)world->CreateJoint(&rJointDef);
			break;
		}
	case 'p': //Prismatic Joint
		{
			body1 = CreateBox(0.0,0.0,8.0,5.0,1.0,0.5,0.5);
			body2 = CreateBox(0,-8.0,8.0,5.0,1.0,0.5,0.5);
			
			body1->SetType(b2_kinematicBody);

			//Para definir um outro eixo (em termos de um vetor normalizado)
			b2Vec2 axis = CalculaComponentesDoVetor(1, 45);
	
			b2PrismaticJointDef pJointDef;
			pJointDef.Initialize(body1, body2, body1->GetWorldCenter(), axis);
			pJointDef.collideConnected = false;

			//Para limitar a translação
			pJointDef.lowerTranslation = -15.0;
			pJointDef.upperTranslation = 15.0;
			pJointDef.enableLimit = true;

			//Para criar um motor
			pJointDef.maxMotorForce = 100.0f;
			pJointDef.motorSpeed = 10.0f;
			pJointDef.enableMotor = true;

			pJoint = (b2PrismaticJoint*)world->CreateJoint(&pJointDef);
			break;
		}
	case 'P': //Pulley Joint
		{
			body1 = CreateBox(-5,0.0,5.0,5.0,3.0,0.5,0.5);
			body2 = CreateBox(5,0.0,5.0,5.0,3.0,0.5,0.5);
			b2Vec2 worldAnchorOnBody1 = body1->GetWorldCenter(); 
			b2Vec2 worldAnchorOnBody2 = body2->GetWorldCenter(); 

			b2Vec2 ground1(worldAnchorOnBody1.x, worldAnchorOnBody1.y+10);
			b2Vec2 ground2(worldAnchorOnBody2.x, worldAnchorOnBody2.y+10);

			b2PulleyJointDef puJointDef;
			puJointDef.Initialize(body1, body2, ground1, ground2, worldAnchorOnBody1,worldAnchorOnBody2, 1.0);
			puJointDef.collideConnected = true;

			pulleyJoint = (b2PulleyJoint*)world->CreateJoint(&puJointDef);
			break;
		}
	case 'g': //Gear Joint
		{
			body1 = CreateBox(-5.0, 0.0, 5.0, 5.0, 1.0, 0.5, 0.5);
			body2 = CreateBox(5.0, 0.0, 5.0, 5.0, 1.0, 0.5, 0.5);
			
			//Criação da junta revoluta
			b2RevoluteJointDef rJointDef;
			rJointDef.Initialize(body1, body2, body1->GetWorldCenter());
			rJointDef.maxMotorTorque = 50.0f; //N-m
			rJointDef.motorSpeed = -10.0f; //radianos por segundo 
			rJointDef.enableMotor = true;
				
			revoluteJoint1 = (b2RevoluteJoint*)world->CreateJoint(&rJointDef);

			//Criação da junta prismática
			b2PrismaticJointDef pJointDef;
			pJointDef.Initialize(chao, body2, body2->GetWorldCenter(), b2Vec2(1, 1));
			pJointDef.collideConnected = true;
			pJoint = (b2PrismaticJoint*)world->CreateJoint(&pJointDef);

			//Criação da junta de engrenagem
			b2GearJointDef gJointDef;
			gJointDef.bodyA = body1;
			gJointDef.bodyB = body2;
			gJointDef.joint1 = revoluteJoint1;
			gJointDef.joint2 = pJoint;
			gJointDef.ratio = b2_pi;  // 2 * b2_pi;
			gearJoint = (b2GearJoint*)world->CreateJoint(&gJointDef);

			break;
		}
	case 'w': // wheel joint
		{ 
			float scaleFactor = 3.0; //fator de escala para o tamanho do carro
			b2PolygonShape chassis;
			b2Vec2 vertices[8];
			vertices[0].Set(-1.5f*scaleFactor, -0.5f*scaleFactor);
			vertices[1].Set(1.5f*scaleFactor, -0.5f*scaleFactor);
			vertices[2].Set(1.5f*scaleFactor, 0.0f*scaleFactor);
			vertices[3].Set(0.0f*scaleFactor, 0.9f*scaleFactor);
			vertices[4].Set(-1.15f*scaleFactor, 0.9f*scaleFactor);
			vertices[5].Set(-1.5f*scaleFactor, 0.2f*scaleFactor);
			chassis.Set(vertices, 6);

			b2CircleShape circle;
			circle.m_radius = 0.4f*scaleFactor;

			//Posição do carro
			float carPosY = -20.0;
			float carPosX = -20.0;

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(carPosX, carPosY);
			car = world->CreateBody(&bd);
			car->CreateFixture(&chassis, 1.0f);

			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 1.0f;
			fd.friction = 0.9f;

			bd.position.Set(carPosX-scaleFactor, carPosY-0.5f*scaleFactor);
			body1 = world->CreateBody(&bd);
			body1->CreateFixture(&fd);

			bd.position.Set(carPosX+scaleFactor, carPosY-0.5f*scaleFactor);
			body2 = world->CreateBody(&bd);
			body2->CreateFixture(&fd);

			//Criação das juntas de rodas
			b2WheelJointDef jd;
			b2Vec2 axis(0.0f, 1.0f);

			jd.Initialize(car, body1, body1->GetPosition(), axis);
			jd.motorSpeed = -30.0f;
			jd.maxMotorTorque = 30.0f;
			jd.enableMotor = true;
			jd.dampingRatio = 0.7;
			wheelJoint1 = (b2WheelJoint*)world->CreateJoint(&jd);

			jd.Initialize(car,body2, body2->GetPosition(), axis);
			//jd.motorSpeed = 0.0f;
			//jd.maxMotorTorque = 10.0f;
			//jd.enableMotor = false;
			jd.dampingRatio = 0.7;
			wheelJoint2 = (b2WheelJoint*) world->CreateJoint(&jd);

			break;
		}
    //Aplica torque sobre o corpo
	case 't':
		{
			//Para aplicar um torque no objeto (girar ao redor do seu centro)
			caixa->ApplyTorque(1000, true);
			//Para aplicar um impulso angular no objeto (quantidade de movimento gerado pela aplicação de um torque)
			caixa->ApplyAngularImpulse(-1000,true);
			break;
		}
	//Sai do programa
	case 27:
		world->~b2World();
		world = NULL;
		
		//Forçando a barra pra sair..
		system("exit");

		exit(0);
		break;

	}
	glutPostRedisplay();
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
void MouseDown(const b2Vec2& worldPoint)
{
	mouseWorld = worldPoint;
	
	if (mouseJoint != NULL)
	{
		return;
	}

	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = worldPoint - d;
	aabb.upperBound = worldPoint + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(worldPoint);
	world->QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();
		b2MouseJointDef mouseJointDef;
		mouseJointDef.bodyA = chao;
		mouseJointDef.bodyB = body;
		mouseJointDef.target = worldPoint;
		mouseJointDef.maxForce = 1000.0f * body->GetMass();
		mouseJointDef.dampingRatio = 1.0;
		mouseJointDef.collideConnected = true;
		mouseJoint = (b2MouseJoint*)world->CreateJoint(&mouseJointDef);
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
			cout << "Screen To World: " << p.x << " " << p.y << endl;
			if(mouseJoint)
				world->DestroyJoint(mouseJoint);
			mouseJoint = NULL;
		}
	}

	glutPostRedisplay();
}

//Para atualizar a junta de mouse quando o mouse se move
void MouseMove(const b2Vec2& p)
{
	mouseWorld = p;
	
	if (mouseJoint)
	{
		mouseJoint->SetTarget(p);
	}

	glutPostRedisplay();
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
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(height, width);
	char title[32];
	sprintf(title, "Box2D Version %d.%d.%d -- Aprendendo Forças", b2_version.major, b2_version.minor, b2_version.revision);
	mainWindow = glutCreateWindow(title);

	glutDisplayFunc(SimulationLoop);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	//Usa um timer para controlar o frame rate.
	glutTimerFunc(framePeriod, Timer, 1);

	//E agora, funções do Mouse!
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	//Rotina com a inicialização do mundo
	InitBox2D();

	//Cria o chão
	b2BodyDef bd;
	chao = world->CreateBody(&bd);
	//Forma do chão: edge
	b2EdgeShape shape;
	shape.Set(b2Vec2(-39.5, -39.5), b2Vec2(39.5, -39.5));
	chao->CreateFixture(&shape,0.0);

	//Cria o teto
	teto = world->CreateBody(&bd);
	shape.Set(b2Vec2(-39.5, 39.5), b2Vec2(39.5, 39.5));
	teto->CreateFixture(&shape,0.0);

	//Cria a parede esquerda
	paredeEsquerda = world->CreateBody(&bd);
	shape.Set(b2Vec2(-39.5, 39.5), b2Vec2(-39.5, -39.5));
	paredeEsquerda->CreateFixture(&shape,0.0);

	//Cria a parede direita
	paredeDireita = world->CreateBody(&bd);
	shape.Set(b2Vec2(39.5, 39.5), b2Vec2(39.5, -39.5));
	paredeDireita->CreateFixture(&shape,0.0);

	caixa = CreateBox(15.0,-35.0,8.0,6.0,3.0,0.5,0.5);
	
	glutMainLoop();

	return 0;
}
