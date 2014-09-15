#include "Settings.h"


void ResizeWithProportion(int32 width, int32 height, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
	// Evita a divisao por zero
	if(height == 0) 
		height = 1;

	GLdouble aspectRadio = height/width;

	// Especifica as dimensões da Viewport
	glViewport(0, 0, width, height);

	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Estabelece a janela de seleção (esquerda, direita, inferior, 
	// superior) mantendo a proporção com a janela de visualização
	if (width <= height) 
		gluOrtho2D (left, right, bottom*aspectRadio, top*aspectRadio);
	else 
		gluOrtho2D (left*aspectRadio, right*aspectRadio, bottom, top);
}
