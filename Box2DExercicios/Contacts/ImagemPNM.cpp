#include "ImagemPNM.h"

#include <iostream>
#include <fstream>

#define BUFF_SIZE 500

using namespace std;

void ImagemPNM::LePNM(char *nomeDoArquivo)
{

	cout << "Lendo a imagem " << nomeDoArquivo << "\n";
	 // Tenta abrir o arquivo...
    ifstream input(nomeDoArquivo, ios::in);
    if (!input) {
        cerr << "Arquivo "<< nomeDoArquivo << " não pode ser aberto!\n";
        exit(1);
    }

    // Cabeçalho, considerando este formato:
    // Px
    // # comentario
    // Width Height
    // 255

   char buffer[BUFF_SIZE];
   input.getline(buffer,BUFF_SIZE); //pega a linha com o Px
   input.getline(buffer,BUFF_SIZE); //pega a linha com o comentário

    // Pega a altura e a largura
   input >> largura;
   input >> altura;
   cout << "Resolucao " << largura << " X " << altura << "\n";
   
   imagem = new Image(largura,altura);

   int max;
   input >> max; //pega o MAX GREY (que pra nós não precisa agora)


   // Percorre os pixels
   for(int y = altura-1; y >= 0; y--)
	   for (int x = 0; x < largura; x++)
	   {
			int r, g, b,a;
			input >> r;
			input >> g;
			input >> b;
			//cout << r << " " << g << " " << b << "\n";
			
			if (r == 255 && g == 0 && b == 255)
				a = 0;
			else a = 255;
			int rgb = (a << 24) | (b << 16) | (g << 8) | r;
			//int rgb = (a << 24) | (r << 16) | (g << 8) | b;
			imagem->setRGB(x, y, rgb);
	   }
	cout << "Leitura realizada com sucesso! \n";
}

void ImagemPNM::CriaPNM(char *nomeDoArquivo)
{
	ofstream arq(nomeDoArquivo, ios::out | ios::binary);

    arq << "P3" << endl;
	arq << "# " << endl;
    arq << largura << " " << altura << endl;
    arq << 255 << endl;

	for(int i=altura; i>=0; i--)
		for(int j=0;  j<largura; j++) {
			int pixel = imagem->getRGB(j,i);
			int b = (pixel >> 16) & 255;
			int g = (pixel >> 8) & 255;
			int r = (pixel) & 255;
			arq << r << " " << g << " " << b << "\n";
		}
	arq.close();
}

void ImagemPNM::ConverteGrayscale()
{
	for(int i=0; i<altura; i++)
		for(int j=0;  j<largura; j++) {
			int pixel = imagem->getRGB(j,i);
			int b = (pixel >> 16) & 255;
			int g = (pixel >> 8) & 255;
			int r = (pixel) & 255;
			
			r = 0.33*r + 0.33*g + 0.33*b;
			g = r;
			b = r;
			int rgb = (b << 16) | (g << 8) | r;
			imagem->setRGB(j,i,rgb);
			//cout << r << " " << g << " " << b << "\n";
		}
}

void ImagemPNM::Coloriza(int r, int g, int b)
{
	int colorizador = (b << 16) | (g << 8) | r;
	cout << "Colorizador: " << colorizador << "\n";
	for(int i=0; i<altura; i++)
		for(int j=0;  j<largura; j++) {
			int pixel = imagem->getRGB(j,i);
			int pixelColorizado = pixel | colorizador;
			imagem->setRGB(j,i,pixelColorizado);
		}
}

void ImagemPNM::Inverte()
{
	for(int i=0; i<altura; i++)
		for(int j=0;  j<largura; j++) {
			int pixel = imagem->getRGB(j,i);
			/*int b = (pixel >> 16) & 255;
			int g = (pixel >> 8) & 255;
			int r = (pixel) & 255;
			b = b ^ 255;
			g = g ^ 255;
			r = r ^ 255;
			int pixelInvertido =  (b << 16) | (g << 8) | r;*/
			int pixelInvertido = pixel ^ 4294967295;
			imagem->setRGB(j,i,pixelInvertido);
		}
}

//Pensar
void ImagemPNM::CriaNormalMap()
{
	for(int i=0; i<altura; i++)
		for(int j=0;  j<largura; j++) {
			int pixel = imagem->getRGB(j,i);
			/*int b = (pixel >> 16) & 255;
			int g = (pixel >> 8) & 255;
			int r = (pixel) & 255;
			b = b ^ 255;
			g = g ^ 255;
			r = r ^ 255;
			int pixelInvertido =  (b << 16) | (g << 8) | r;*/
			int pixelInvertido = pixel ^ 4294967295;
			imagem->setRGB(j,i,pixelInvertido);
		}
}