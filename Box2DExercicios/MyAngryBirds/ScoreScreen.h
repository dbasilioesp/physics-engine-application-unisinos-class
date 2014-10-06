#pragma once
#include <Box2D/Box2D.h>


struct ScoreScreen {
	ScoreScreen(){
		score = 0;
		position.x = 0;
		position.y = 0;
		duration = 2;  // 2 seconds
	}
	int score;
	float duration;
	b2Vec2 position;
};