#include "pch.h"
#include "MeleeEnemy.h"


MeleeEnemy::MeleeEnemy(float size, float speed, const Color4f& color) :
	 m_MoveVector{}, m_Size{ size }, m_Speed{ speed }, m_Active{false}, m_Color{color}
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

void MeleeEnemy::Draw()
{
	utils::SetColor(m_Color);
	if(m_Active)
	{
		utils::FillEllipse(m_Pos, m_Size, m_Size);
	}
}

void MeleeEnemy::Update(Point2f targetPos, float elapsedSec)
{
	if (m_Active)
	{
		Vector2f vector{ targetPos.x - m_Pos.x,targetPos.y - m_Pos.y };
		vector /= vector.Norm();
		m_Pos.x += vector.x * m_Speed * elapsedSec;
		m_Pos.y += vector.y * m_Speed * elapsedSec;
	}
}

void MeleeEnemy::SetActive()
{
	m_Active = true;
}

bool MeleeEnemy::CheckHit(const Circlef& target)
{
	float xDistance{ m_Pos.x - target.center.x};
	float yDistance{ m_Pos.y - target.center.y };
	float Distance{ float(sqrt(xDistance * xDistance + yDistance * yDistance)) };

	return (Distance < target.radius + m_Size);
}
void MeleeEnemy::Reset()
{
	m_MoveVector = Vector2f{ 0.f,0.f };

	m_Pos.x = float((rand() % (1001 - 77)) - 77);
	if (m_Pos.x < -10 || m_Pos.x > 856)
	{
		m_Pos.y = float(rand() % 501);
	}
	else
	{
		m_Pos.y = 520.f;
	}
}