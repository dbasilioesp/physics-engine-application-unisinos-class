#include "Settings.h"


//Rotina de Callback de redimensionamento da janela 
void ResizeWithProportion(int32 w, int32 h, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
	GLsizei width, height;

	// Evita a divisao por zero
	if(h == 0) h = 1;

	// Atualiza as variáveis
	width = w;
	height = h;
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
