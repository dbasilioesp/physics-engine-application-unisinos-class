#include "Image.h"

#include <cstdio>

class ImagemPNM
{

public:
	ImagemPNM() { imagem = NULL; altura = 0; largura = 0;}
	~ImagemPNM() {;}

	void LePNM(char *nomeDoArquivo);
	void CriaPNM(char *nomeDoArquivo);
	void ConverteGrayscale();
	void Coloriza(int r, int g, int b);
	void Inverte();
	void Binariza(int k);
	void CriaNormalMap();

	Image *RetornaImagem() 
	{
		return imagem;
	}
	void RecebeImagem(Image *im)
	{
		imagem = im;
	}
	int RetornaLargura() { return largura; }
	int RetornaAltura() { return altura; }

private:
	Image *imagem;
	int altura, largura;

};