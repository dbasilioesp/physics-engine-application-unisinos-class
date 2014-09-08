
#include <Box2D/Box2D.h>
#include <GL/glut.h>

#include <cstdio>
#include <iostream>


//Rotina que calcula a norma (módulo) de um vetor
float norma(b2Vec2 v);

//Rotina que normaliza um vetor
b2Vec2 normaliza(b2Vec2 v);

//Rotina que converte graus para radianos
float GrausParaRadianos(float angulo);

//Rotina que converte radianos para graus
float RadianosParaGraus(float angle);

//Rotina que calcula as componentes vx e vy do vetor formado pelas posicoes x e y e um angul//Rotina que calcula as componentes vx e vy do vetor formado pelas posicoes x e y e um angulo
b2Vec2 CalculaComponentesDoVetor(float x, float y, float angulo);

//Rotina que calcula as componentes vx e vy do vetor formado pelo comprimento (magnitude) fornecido e o ângulo
b2Vec2 CalculaComponentesDoVetor(float magnitude, float angulo);
