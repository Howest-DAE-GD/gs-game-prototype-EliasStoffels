#pragma once
#include "utils.h"

class RangedEnemy
{
public:
	RangedEnemy(float size, float speed,float bulletSize,float bulletSpeed, const Color4f& color);
	void Draw();
	void Update(Point2f targetPos, float elapsedSec);
	void SetActive();
	bool CheckHitBody(const Circlef& target) const;
	bool CheckHitBullet(const Circlef& target) const;
	void Reset();
private:
	Point2f m_Pos;
	Point2f m_BulletPos;
	Vector2f m_AimVector;
	Color4f m_Color;
	float m_Size;
	float m_BulletSize;
	float m_BulletSpeed;
	float m_Speed;
	bool m_Active;

	const float AIM_DELAY;
	const float SHOOT_DELAY;
	float m_TotalElapsedSec;

	

	enum class CombatState
	{
		Moving,Aiming,Shooting
	};
	CombatState m_CombatState;
};

