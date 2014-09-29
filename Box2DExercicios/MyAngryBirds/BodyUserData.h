#pragma once
#include <iostream>
#include <string>
#include "Box2D\Box2D.h"


using namespace std;


struct BodyUserData {
	BodyUserData(){
		entityType = "scenary";
		health = 0;
	}
	string entityType;
	float health;
};