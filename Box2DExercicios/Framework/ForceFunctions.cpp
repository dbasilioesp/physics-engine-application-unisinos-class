#include "ForceFunctions.h"


// Rotina que calcula a normal (módulo) de um vetor
float norma(b2Vec2 v) {
      return sqrt(v.x*v.x + v.y*v.y);
}


// Rotina que normaliza um vetor
b2Vec2 normaliza(b2Vec2 v)
{
	float normav = norma(v);
	v.x = v.x / normav;
	v.y = v.y / normav;
	return v;
}


//Rotina que converte graus para radianos
float GrausParaRadianos(float angulo)
{
	return angulo*b2_pi/180;
}


//Rotina que converte radianos para graus
float RadianosParaGraus(float angle){
    return angle*180/b2_pi;
}


//Rotina que calcula as componentes vx e vy do vetor formado pelas posicoes x e y e um angul//Rotina que calcula as componentes vx e vy do vetor formado pelas posicoes x e y e um angulo
b2Vec2 CalculaComponentesDoVetor(float x, float y, float angulo)
{
	float v = sqrt(x*x+y*y);
	float angulorad = GrausParaRadianos(angulo);
	float vx = v*cos(angulorad);
	float vy = v*sin(angulorad);
	b2Vec2 vec(vx,vy);
	return vec;
}


//Rotina que calcula as componentes vx e vy do vetor formado pelo comprimento (magnitude) fornecido e o ângulo
b2Vec2 CalculaComponentesDoVetor(float magnitude, float angulo)
{
	float v = magnitude;
	float angulorad = GrausParaRadianos(angulo);
	float vx = v*cos(angulorad);
	float vy = v*sin(angulorad);
	b2Vec2 vec(vx,vy);
	return vec;
}


b2Vec2 ConvertScreenToWorld(int32 x, int32 y, int width, int height)
{
	float32 u = x / float32(width);
	float32 v = (height - y) / float32(height);

	float32 ratio = float32(width) / float32(height);
	b2Vec2 extents(ratio * 40.0f, 40.0f); 
	
	b2Vec2 lower = -extents;
	b2Vec2 upper = extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
}