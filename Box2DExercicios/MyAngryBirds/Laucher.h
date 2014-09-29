#pragma once
#include <iostream>
#include "Box2D\Box2D.h"


using namespace std;


struct Laucher {
	Laucher(){
		x = -80.0f;
		y = -35.0f;
		angle = 45.0;
	}
	float32 x;
	float32 y;
	float32 angle;
};