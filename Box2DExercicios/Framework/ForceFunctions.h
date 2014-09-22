#include <Box2D/Box2D.h>
#include <GL/glut.h>
#include <cstdio>
#include <iostream>


float norma(b2Vec2 v);


b2Vec2 normaliza(b2Vec2 v);


float GrausParaRadianos(float angulo);


float RadianosParaGraus(float angle);


b2Vec2 CalculaComponentesDoVetor(float x, float y, float angulo);


b2Vec2 CalculaComponentesDoVetor(float magnitude, float angulo);
