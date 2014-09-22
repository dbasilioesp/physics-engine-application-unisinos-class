#pragma once

class Image {
public:
	Image(int width, int height){
		this->width = width;
		this->height = height;
		pixels = new int[width*height];
	}
	~Image(void){
		delete[] pixels;
	}

	void setRGB(int x, int y, int rgb){
		pixels[x + y * width] = rgb;
	}

	int getRGB(int x, int y){
		return pixels[x + y * width];
	}

	int getWidth(){ return width; }
	int getHeight(){ return height; }

	int* getPixels(){ return pixels; }

private:
	int width, height;
	int *pixels;
};
