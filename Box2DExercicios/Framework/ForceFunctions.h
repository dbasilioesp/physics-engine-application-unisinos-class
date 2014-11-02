#include <Box2D/Box2D.h>
#include <GL/glut.h>
#include <cstdio>
#include <iostream>


float norma(b2Vec2 v);


b2Vec2 normaliza(b2Vec2 v);


float GrausParaRadianos(float angulo);


float RadianosParaGraus(float angle);


b2Vec2 CalculaComponentesDoVetor(float x, float y, float angulo);


b2Vec2 CalculaComponentesDoVetor(float magnitude, float angulo);


b2Vec2 ConvertScreenToWorld(int32 x, int32 y, int width, int height);


class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;

				// We are done, terminate the query.
				return false;
			}
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};