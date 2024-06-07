#include "pch.h"
#include "RangedEnemy.h"
#include <iostream>
RangedEnemy::RangedEnemy(float size, float speed, float bulletSize, float bulletSpeed, const Color4f& color):
	m_AimVector{}, m_Size{ size }, m_Speed{ speed }, m_Active{ false }, m_Color{ color }, m_CombatState{CombatState::Moving},

	AIM_DELAY{3.f},SHOOT_DELAY{5.f},m_TotalElapsedSec{}, m_BulletPos{},m_BulletSize{bulletSize}, m_BulletSpeed{bulletSpeed}
{
	m_Pos.x = float((rand() % 2539) - 846);
	if (m_Pos.x < -10)
	{
		m_Pos.y = float(rand() % 501);
		m_Pos.x = -20;
	}
	else if (m_Pos.x > 856)
	{
		m_Pos.y = float(rand() % 501);
		m_Pos.x = 876;
	}
	else
	{
		m_Pos.y = 520.f;
	}
}

void RangedEnemy::Draw()
{
	utils::SetColor(m_Color);
	if (m_Active)
	{
		utils::FillEllipse(m_Pos, m_Size, m_Size);
	}
	if (m_CombatState == CombatState::Aiming)
	{
		utils::SetColor(Color4f{ 1.f,1.f,1.f, .3f });
		utils::DrawLine(m_Pos, m_Pos + m_AimVector);
	}
	if (m_CombatState == CombatState::Shooting)
	{
		utils::FillEllipse(m_BulletPos, m_BulletSize, m_BulletSize);
	}
}

void RangedEnemy::Update(Point2f targetPos, float elapsedSec)
{
	if(m_CombatState != CombatState::Shooting&&m_Active)
	{
		m_TotalElapsedSec += elapsedSec;
	}
	if (m_TotalElapsedSec > SHOOT_DELAY)
	{
		m_CombatState = CombatState::Shooting;
		m_TotalElapsedSec = 0.f;
		m_BulletPos = m_Pos;
	}
	else if (m_TotalElapsedSec > AIM_DELAY)
	{
		m_CombatState = CombatState::Aiming;
	}

	if (m_Active)
	{
		switch (m_CombatState)
		{
		case CombatState::Moving:
		{
			Vector2f vector{ targetPos.x - m_Pos.x,targetPos.y - m_Pos.y };
			vector /= vector.Norm();
			m_Pos.x += vector.x * m_Speed * elapsedSec;
			m_Pos.y += vector.y * m_Speed * elapsedSec;
		}
		break;
		case CombatState::Aiming:
		{
			Vector2f vector{ targetPos.x - m_Pos.x,targetPos.y - m_Pos.y };
			vector *= 20;
			m_AimVector = vector;
		}
		break;
		case CombatState::Shooting:
		{
			Vector2f aimVectorN{ m_AimVector/m_AimVector.Norm()};
			m_BulletPos.x += aimVectorN.x * m_BulletSpeed * elapsedSec;
			m_BulletPos.y += aimVectorN.y * m_BulletSpeed * elapsedSec;

			if (m_BulletPos.x < 0.f || m_BulletPos.x > 900.f || m_BulletPos.y < 0.f || m_BulletPos.y> 600.f)
			{
				m_CombatState = CombatState::Moving;
			}
		}
		break;
		}
		
	}
}

void RangedEnemy::SetActive()
{
	m_Active = true;
}

bool RangedEnemy::CheckHitBody(const Circlef& target) const
{
	float xDistance{ m_Pos.x - target.center.x };
	float yDistance{ m_Pos.y - target.center.y };
	float Distance{ float(sqrt(xDistance * xDistance + yDistance * yDistance)) };

	return (Distance < target.radius + m_Size);
}

bool RangedEnemy::CheckHitBullet(const Circlef& target) const
{
	float xDistance{ m_BulletPos.x - target.center.x };
	float yDistance{ m_BulletPos.y - target.center.y };
	float Distance{ float(sqrt(xDistance * xDistance + yDistance * yDistance)) };

	return (Distance < target.radius + m_BulletSize);
}

void RangedEnemy::Reset()
{
	m_AimVector = Vector2f{ 0.f,0.f };

	m_Pos.x = float((rand() % (1001 - 77)) - 77);
	if (m_Pos.x < -10 || m_Pos.x > 856)
	{
		m_Pos.y = float(rand() % 501);
	}
	else
	{
		int temp{ rand() % 2 };
		if (temp == 0)
		{
			m_Pos.y = 520.f;
		}
		else
		{
			m_Pos.y = -20.f;
		}

	}
	m_CombatState = CombatState::Moving;
}
