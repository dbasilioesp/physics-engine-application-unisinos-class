#pragma once
#include <iostream>
#include <string>
#include "Box2D\Box2D.h"


using namespace std;


struct BodyUserData {
	BodyUserData(){
		entityType = "scenary";
		health = 1.0;
		color.r = 1.0;
		color.g = 0.0;
		color.b = 0.0;
	}
	string entityType;
	float health;
	b2Color color;
};