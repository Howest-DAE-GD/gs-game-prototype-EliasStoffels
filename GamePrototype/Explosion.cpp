#include "pch.h"
#include "Explosion.h"
#include <utils.h>
#include <iostream>

Explosion::Explosion(const Circlef& explosion):
	m_Explosion{explosion}, m_Expansion{explosion.radius*1.5f}, m_Opacity{1.f}
{

}

void Explosion::Draw(bool explode)
{
	if(explode)
	{
		utils::SetColor(Color4f{ 1.f,1.f,1.f,m_Opacity });
		utils::FillEllipse(m_Explosion.center, m_Explosion.radius, m_Explosion.radius);
		utils::SetColor(Color4f{ 0.f, 0.f, 0.f,1.f });
		utils::FillEllipse(m_Explosion.center, m_Explosion.radius - 5.f, m_Explosion.radius - 5.f);
	}
}

void Explosion::Update(float elapsedSec, const Circlef& explosion, bool explode)
{
	m_Expansion = explosion.radius*1.5f;
	if(explode)
	{
		m_Explosion.radius += m_Expansion * elapsedSec;
		m_Opacity -= 0.3f * elapsedSec;
	}
}

Circlef Explosion::GetHitBox()
{
	return m_Explosion;
}

void Explosion::Reset(const Circlef& explosion)
{
	m_Explosion = explosion;
	m_Expansion = m_Explosion.radius*1.5f;
	m_Opacity = 1.0f;
}