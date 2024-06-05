#pragma once
#include "utils.h"

class MeleeEnemy
{
public:
	MeleeEnemy(float size, float speed,const Color4f& color);
	void Draw();
	void Update(Point2f targetPos, float elapsedSec);
	void SetActive();
	bool CheckHit(const Circlef& target);
	void Reset();

private:
	Point2f m_Pos;
	Vector2f m_MoveVector;
	Color4f m_Color;
	float m_Size;
	float m_Speed;
	bool m_Active;
};

