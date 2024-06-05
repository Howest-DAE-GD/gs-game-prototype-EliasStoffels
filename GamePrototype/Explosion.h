#pragma once
class Explosion
{
public:
	Explosion(const Circlef& explosion);
	void Draw(bool explode);
	void Update(float elapsedSec, const Circlef& explosion, bool explode);
	Circlef GetHitBox();
	void Reset(const Circlef& explosion);

private:
	Circlef m_Explosion;
	float m_Expansion;
	float m_Opacity;

};

