#include "pch.h"
#include "Game.h"
#include <utils.h>
#include <iostream>

Game::Game(const Window& window)
	:BaseGame{ window }, m_MainChar1{ 50.f,300.f,20.f }, m_MainChar2{ 50.f,200.f,20.f }, m_MainChar1State{ MovementState::Still }, m_MainChar2State{ MovementState::Still },
	m_MainChar1Speed{ 100.f }, m_MainChar2Speed{ 100.f }, MAX_HP{5}, m_CurrentHpGreen{5}, m_CurrentHpRed{5}, m_AliveState{AliveState::Both},

	m_NrMelees{5}, m_RedMeleeEnemysVector{5}, m_GreenMeleeEnemysVector{ 5 }, explode{false}, m_ExplosionDuration{}, MAX_EXPLOSION_DURATION{3.f}, m_ExplosionOnCD{false}, m_LastExplosionElapsed{}, EXPLOSION_CD{10.f},

	m_MenueState{MenueState::Start}, REVIVE_TIME{5.f}, m_CurrentReviveTime{}, m_NrRanged{3}, m_RedRangedEnemysVector{ 5 }, m_GreenRangedEnemysVector{ 5 }, RANGED_SPAWN_TIME{5.f}, m_TotalElapsedSec{},

	m_RangedSpawn{false}, m_NrEnemiesDefeated{}, m_EndMessageSend{false}

{
	Initialize();
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		m_RedMeleeEnemysVector[counter]->SetActive();
	}
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		m_GreenMeleeEnemysVector[counter]->SetActive();
	}

	

	m_MC1Explosion = new Explosion(m_MainChar1);
	m_MC2Explosion = new Explosion(m_MainChar2);
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{

	std::cout << "Made with azerty sorry :(\nRed:\nZ: Move Up\nQ: move Left\nS: Move down\nd: Move Right\n\nGreen:\nArrow keys to move\n\nA/E: Shrink/Grow\nR: When you see a white circle in the top middle of the screen -> Explode\n\nHitting enemies of the same colour defeats them while hitting enemies of the other colour makes you take 1 dmg\n\nPress any of the controls to start\nIf game over Press space to restart\n";
	std::cout << "\nIf one of you dies the other can revive them by staying on their body until the bar next to their hp is fully filled but be carefull a dead person can't take out enemies anymore";

	m_SizeChange = m_MainChar1.radius / 4.f;
	m_MinRadius = m_MainChar1.radius / 2.f + 1.f;
	
	m_SpeedChange = m_MainChar1Speed / 4.f;

	//ranged
	for (int counter{}; counter < m_NrRanged;++counter)
	{
		m_RedRangedEnemysVector[counter] = new RangedEnemy(7.5f, 75.f, 5.f, 100.f, Color4f{ 1.f,0.f,0.f,1.f });
	}
	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		m_GreenRangedEnemysVector[counter] = new RangedEnemy(7.5f, 75.f, 5.f, 100.f, Color4f{ 0.f,1.f,0.f,1.f });
	}

	//melees
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		m_RedMeleeEnemysVector[counter] = new MeleeEnemy(10.f, 50.f, Color4f{ 1.f,0.f,0.f,1.f });
	}
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		m_GreenMeleeEnemysVector[counter] = new MeleeEnemy(10.f, 50.f, Color4f{ 0.f,1.f,0.f,1.f });
	}


	std::cout << GetViewPort().height << std::endl << GetViewPort().width;
}

void Game::Cleanup()
{
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		delete m_RedMeleeEnemysVector[counter];
	}
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		delete m_GreenMeleeEnemysVector[counter];
	}

	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		delete m_RedRangedEnemysVector[counter];
	}
	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		delete m_GreenRangedEnemysVector[counter];
	}
	delete m_MC1Explosion;
	delete m_MC2Explosion;
}

void Game::Update( float elapsedSec )
{

	m_TotalElapsedSec += elapsedSec;
	if (m_TotalElapsedSec > RANGED_SPAWN_TIME && m_RangedSpawn ==false)
	{
		m_RangedSpawn = true;
		for (int counter{}; counter < m_NrRanged; ++counter)
		{
			m_RedRangedEnemysVector[counter]->SetActive();
		}
		for (int counter{}; counter < m_NrRanged; ++counter)
		{
			m_GreenRangedEnemysVector[counter]->SetActive();
		}
	}

	if(m_MenueState == MenueState::Game)
	{
		if (explode)
		{
			m_ExplosionDuration += elapsedSec;
			if (m_ExplosionDuration >= MAX_EXPLOSION_DURATION)
			{
				m_MC1Explosion->Reset(m_MainChar1);
				m_MC2Explosion->Reset(m_MainChar2);
				explode = false;
			}
		}

		if (m_ExplosionOnCD)
		{
			m_LastExplosionElapsed += elapsedSec;
			if (m_LastExplosionElapsed >= EXPLOSION_CD)
			{
				m_ExplosionOnCD = false;
			}
		}

		UpdateMainChars(elapsedSec);

		//melees
		UpdateMelees(elapsedSec);

		//Ranged
		UpdateRanged(elapsedSec);

		m_MC1Explosion->Update(elapsedSec, m_MainChar1, explode);
		m_MC2Explosion->Update(elapsedSec, m_MainChar2, explode);
	}

	if (m_AliveState == AliveState::Neither)
	{
		m_MenueState = MenueState::GameOver;
		if(!m_EndMessageSend)
		{
			std::cout << "GAME OVER\n" << m_NrEnemiesDefeated << "enemies defeated\n";
			m_EndMessageSend = true;
		}
	}
}

void Game::UpdateMainChars( float elapsedSec)
{

	if (m_AliveState == AliveState::Both || m_AliveState == AliveState::Green)
	{
		switch (m_MainChar1State)
		{
		case MovementState::Up:
			m_MainChar1.center.y += m_MainChar1Speed * elapsedSec;
			break;
		case MovementState::Down:
			m_MainChar1.center.y -= m_MainChar1Speed * elapsedSec;
			break;
		case MovementState::Left:
			m_MainChar1.center.x -= m_MainChar1Speed * elapsedSec;
			break;
		case MovementState::Right:
			m_MainChar1.center.x += m_MainChar1Speed * elapsedSec;
			break;
		}
	}

	if (m_MainChar1.center.x - m_MainChar1.radius < GetViewPort().left)
	{
		m_MainChar1.center.x = GetViewPort().left + m_MainChar1.radius;
	}
	if (m_MainChar1.center.x + m_MainChar1.radius > GetViewPort().left + GetViewPort().width)
	{
		m_MainChar1.center.x = GetViewPort().left + GetViewPort().width - m_MainChar1.radius;
	}
	if (m_MainChar1.center.y - m_MainChar1.radius < GetViewPort().bottom)
	{
		m_MainChar1.center.y = GetViewPort().bottom + m_MainChar1.radius;
	}
	if (m_MainChar1.center.y + m_MainChar1.radius > GetViewPort().bottom + GetViewPort().height)
	{
		m_MainChar1.center.y = GetViewPort().bottom + GetViewPort().height - m_MainChar1.radius;
	}

	if (m_AliveState == AliveState::Both || m_AliveState == AliveState::Red)
	{
		switch (m_MainChar2State)
		{
		case MovementState::Up:
			m_MainChar2.center.y += m_MainChar2Speed * elapsedSec;
			break;
		case MovementState::Down:
			m_MainChar2.center.y -= m_MainChar2Speed * elapsedSec;
			break;
		case MovementState::Left:
			m_MainChar2.center.x -= m_MainChar2Speed * elapsedSec;
			break;
		case MovementState::Right:
			m_MainChar2.center.x += m_MainChar2Speed * elapsedSec;
			break;
		}
	}

	if (m_MainChar2.center.x - m_MainChar2.radius < GetViewPort().left)
	{
		m_MainChar2.center.x = GetViewPort().left + m_MainChar2.radius;
	}
	if (m_MainChar2.center.x + m_MainChar2.radius > GetViewPort().left + GetViewPort().width)
	{
		m_MainChar2.center.x = GetViewPort().left + GetViewPort().width - m_MainChar2.radius;
	}
	if (m_MainChar2.center.y - m_MainChar2.radius < GetViewPort().bottom)
	{
		m_MainChar2.center.y = GetViewPort().bottom + m_MainChar2.radius;
	}
	if (m_MainChar2.center.y + m_MainChar2.radius > GetViewPort().bottom + GetViewPort().height)
	{
		m_MainChar2.center.y = GetViewPort().bottom + GetViewPort().height - m_MainChar2.radius;
	}

	if (m_AliveState != AliveState::Both&&m_AliveState != AliveState::Neither)
	{
		float xDistance{ m_MainChar1.center.x - m_MainChar2.center.x };
		float yDistance{ m_MainChar1.center.y - m_MainChar2.center.y };
		float Distance{ float(sqrt(xDistance * xDistance + yDistance * yDistance)) };

		if (Distance < m_MainChar1.radius + m_MainChar2.radius)
		{
			m_CurrentReviveTime += elapsedSec;
			if (m_CurrentReviveTime > REVIVE_TIME)
			{
				if (m_CurrentHpRed <= 0)
				{
					m_CurrentHpRed = 5;
				}
				if  (m_CurrentHpGreen <= 0)
				{
					m_CurrentHpGreen = 5;
				}
				m_AliveState = AliveState::Both;
				m_CurrentReviveTime = 0.f;
			}
		}
	}
}

void Game::UpdateMelees(float elapsedSec)
{
	//red
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		m_RedMeleeEnemysVector[counter]->Update(m_MainChar1.center, elapsedSec);
	}

	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		if (m_RedMeleeEnemysVector[counter]->CheckHit(m_MainChar2)&&m_AliveState==AliveState::Both||
			m_RedMeleeEnemysVector[counter]->CheckHit(m_MainChar2) && m_AliveState == AliveState::Red||
			m_RedMeleeEnemysVector[counter]->CheckHit(m_MC1Explosion->GetHitBox())|| 
			m_RedMeleeEnemysVector[counter]->CheckHit(m_MC2Explosion->GetHitBox()))
		{
			m_RedMeleeEnemysVector[counter]->Reset();
			++m_NrEnemiesDefeated;
		}
		if (m_RedMeleeEnemysVector[counter]->CheckHit(m_MainChar1))
		{
			m_CurrentHpGreen -= 1;
			m_RedMeleeEnemysVector[counter]->Reset();
			if (m_CurrentHpGreen <= 0)
			{
				if (m_AliveState == AliveState::Both)
				{
					m_AliveState = AliveState::Red;
				}
				if (m_AliveState == AliveState::Green)
				{
					m_AliveState = AliveState::Neither;
				}
			}
		}
	}

	//green
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		m_GreenMeleeEnemysVector[counter]->Update(m_MainChar2.center, elapsedSec);
	}

	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		if (m_GreenMeleeEnemysVector[counter]->CheckHit(m_MainChar1)&&m_AliveState==AliveState::Both|| 
			m_GreenMeleeEnemysVector[counter]->CheckHit(m_MainChar1) && m_AliveState == AliveState::Green||
			m_GreenMeleeEnemysVector[counter]->CheckHit(m_MC1Explosion->GetHitBox()) ||
			m_GreenMeleeEnemysVector[counter]->CheckHit(m_MC2Explosion->GetHitBox()))
		{
			m_GreenMeleeEnemysVector[counter]->Reset();
			++m_NrEnemiesDefeated;
		}
		if (m_GreenMeleeEnemysVector[counter]->CheckHit(m_MainChar2))
		{
			m_CurrentHpRed -= 1;
			m_GreenMeleeEnemysVector[counter]->Reset();
			if (m_CurrentHpRed <= 0)
			{
				if (m_AliveState == AliveState::Both)
				{
					m_AliveState = AliveState::Green;
				}
				if (m_AliveState == AliveState::Red)
				{
					m_AliveState = AliveState::Neither;
				}
			}
		}
	}
}

void Game::UpdateRanged(float elapsedSec)
{
	//red
	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		m_RedRangedEnemysVector[counter]->Update(m_MainChar1.center, elapsedSec);
	}

	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		if (m_RedRangedEnemysVector[counter]->CheckHitBody(m_MainChar2) && m_AliveState == AliveState::Both ||
			m_RedRangedEnemysVector[counter]->CheckHitBody(m_MainChar2) && m_AliveState == AliveState::Red ||
			m_RedRangedEnemysVector[counter]->CheckHitBody(m_MC1Explosion->GetHitBox()) ||
			m_RedRangedEnemysVector[counter]->CheckHitBody(m_MC2Explosion->GetHitBox()))
		{
			m_RedRangedEnemysVector[counter]->Reset();
			++m_NrEnemiesDefeated;
		}
		if (m_RedRangedEnemysVector[counter]->CheckHitBody(m_MainChar1))
		{
			m_CurrentHpGreen -= 1;
			m_RedRangedEnemysVector[counter]->Reset();
			if (m_CurrentHpGreen <= 0)
			{
				if (m_AliveState == AliveState::Both)
				{
					m_AliveState = AliveState::Red;
				}
				if (m_AliveState == AliveState::Green)
				{
					m_AliveState = AliveState::Neither;
				}
			}
		}
		if (m_RedRangedEnemysVector[counter]->CheckHitBullet(m_MainChar1))
		{
			m_CurrentHpGreen -= 1;
			if (m_CurrentHpGreen <= 0)
			{
				if (m_AliveState == AliveState::Both)
				{
					m_AliveState = AliveState::Red;
				}
				if (m_AliveState == AliveState::Green)
				{
					m_AliveState = AliveState::Neither;
				}
			}
		}
	}

	//green
	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		m_GreenRangedEnemysVector[counter]->Update(m_MainChar2.center, elapsedSec);
	}

	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		if (m_GreenRangedEnemysVector[counter]->CheckHitBody(m_MainChar1) && m_AliveState == AliveState::Both ||
			m_GreenRangedEnemysVector[counter]->CheckHitBody(m_MainChar1) && m_AliveState == AliveState::Green ||
			m_GreenRangedEnemysVector[counter]->CheckHitBody(m_MC1Explosion->GetHitBox()) ||
			m_GreenRangedEnemysVector[counter]->CheckHitBody(m_MC2Explosion->GetHitBox()))
		{
			m_GreenRangedEnemysVector[counter]->Reset();
			++m_NrEnemiesDefeated;
		}
		if (m_GreenRangedEnemysVector[counter]->CheckHitBody(m_MainChar2))
		{
			m_CurrentHpRed -= 1;
			m_GreenRangedEnemysVector[counter]->Reset();
			if (m_CurrentHpRed <= 0)
			{
				if (m_AliveState == AliveState::Both)
				{
					m_AliveState = AliveState::Green;
				}
				if (m_AliveState == AliveState::Red)
				{
					m_AliveState = AliveState::Neither;
				}
			}
		}
		if (m_GreenRangedEnemysVector[counter]->CheckHitBullet(m_MainChar2))
		{
			m_CurrentHpRed -= 1;
			if (m_CurrentHpRed <= 0)
			{
				if (m_AliveState == AliveState::Both)
				{
					m_AliveState = AliveState::Green;
				}
				if (m_AliveState == AliveState::Red)
				{
					m_AliveState = AliveState::Neither;
				}
			}
		}
	}
}

void Game::Draw( ) const
{
	ClearBackground();

	m_MC1Explosion->Draw(explode);
	m_MC2Explosion->Draw(explode);


	DrawEnemies();
	DrawMainChars();

	DrawHp();
}
void Game::DrawMainChars() const
{
	
	if (m_AliveState != AliveState::Both)
	{
		int drawPosX{};
		if (m_CurrentHpGreen <= 0)
		{
			drawPosX = 40;
		}
		else
		{
			drawPosX = GetViewPort().width - 90;
		}
		Rectf reviveRect{ float(drawPosX), GetViewPort().height - 22, 50,10 };
		Rectf currentReviveRect{float(drawPosX), GetViewPort().height - 22, 50 * m_CurrentReviveTime/REVIVE_TIME,10 };
		utils::SetColor(Color4f{ 1.f,1.f,1.f,1.f });
		utils::DrawRect(reviveRect);
		utils::FillRect(currentReviveRect);
	}

	if(m_AliveState==AliveState::Both || m_AliveState==AliveState::Green)
	{
		utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
		utils::FillEllipse(m_MainChar1.center.x, m_MainChar1.center.y, m_MainChar1.radius, m_MainChar1.radius);
		utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 1.f });
		utils::DrawEllipse(m_MainChar1.center.x, m_MainChar1.center.y, m_MainChar1.radius/2, m_MainChar1.radius/2);
	}

	if (m_AliveState == AliveState::Both || m_AliveState == AliveState::Red)
	{
		utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
		utils::FillEllipse(m_MainChar2.center.x, m_MainChar2.center.y, m_MainChar2.radius, m_MainChar2.radius);
		utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 1.f });
		utils::DrawEllipse(m_MainChar2.center.x, m_MainChar2.center.y, m_MainChar2.radius /2, m_MainChar2.radius/2);
	}

	if (m_AliveState == AliveState::Green)
	{
		utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
		utils::FillEllipse(m_MainChar2.center.x, m_MainChar2.center.y, m_MainChar2.radius, m_MainChar2.radius);
		utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 1.f });
		utils::DrawLine(Point2f{ m_MainChar2.center.x - m_MainChar2.radius * 0.5f,m_MainChar2.center.y + m_MainChar2.radius * 0.5f },
			Point2f{ m_MainChar2.center.x + m_MainChar2.radius * 0.5f, m_MainChar2.center.y - m_MainChar2.radius * 0.5f });

		utils::DrawLine(Point2f{ m_MainChar2.center.x + m_MainChar2.radius * 0.5f,m_MainChar2.center.y + m_MainChar2.radius * 0.5f },
			Point2f{ m_MainChar2.center.x - m_MainChar2.radius * 0.5f, m_MainChar2.center.y - m_MainChar2.radius * 0.5f });
	}

	if (m_AliveState == AliveState::Red)
	{
		utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
		utils::FillEllipse(m_MainChar1.center.x, m_MainChar1.center.y, m_MainChar1.radius, m_MainChar1.radius);
		utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 1.f });
		utils::DrawLine(Point2f{ m_MainChar1.center.x - m_MainChar1.radius * 0.5f,m_MainChar1.center.y + m_MainChar1.radius * 0.5f },
			Point2f{ m_MainChar1.center.x + m_MainChar1.radius * 0.5f, m_MainChar1.center.y - m_MainChar1.radius * 0.5f });

		utils::DrawLine(Point2f{ m_MainChar1.center.x + m_MainChar1.radius * 0.5f,m_MainChar1.center.y + m_MainChar1.radius * 0.5f },
			Point2f{ m_MainChar1.center.x - m_MainChar1.radius * 0.5f, m_MainChar1.center.y - m_MainChar1.radius * 0.5f });
	}

	
}
void Game::DrawEnemies() const
{
	//melees
	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		m_RedMeleeEnemysVector[counter]->Draw();
	}

	for (int counter{}; counter < m_NrMelees; ++counter)
	{
		m_GreenMeleeEnemysVector[counter]->Draw();
	}

	//ranged
	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		m_RedRangedEnemysVector[counter]->Draw();
	}

	for (int counter{}; counter < m_NrRanged; ++counter)
	{
		m_GreenRangedEnemysVector[counter]->Draw();
	}
}
void Game::DrawHp() const
{
	for (int emptyCounter{}; emptyCounter < MAX_HP; ++emptyCounter)
	{
		utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
		utils::DrawEllipse(20.f, GetViewPort().height - 20.f - emptyCounter * 20, 5.f, 5.f);
		utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
		utils::DrawEllipse(GetViewPort().width - 30.f, GetViewPort().height - 20.f - emptyCounter * 20, 5.f, 5.f);
	}
	for (int filledCounter{}; filledCounter < m_CurrentHpGreen; ++filledCounter)
	{
		utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
		utils::FillEllipse(20.f, GetViewPort().height - 20.f - filledCounter * 20, 5.f, 5.f);
	}
	for (int filledCounter{}; filledCounter < m_CurrentHpRed; ++filledCounter)
	{
		utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
		utils::FillEllipse(GetViewPort().width - 30.f, GetViewPort().height - 20.f - filledCounter * 20, 5.f, 5.f);
	}

	if (!m_ExplosionOnCD)
	{
		utils::SetColor(Color4f{ 1.f,1.f,1.f,1.f });
		utils::FillEllipse(GetViewPort().width/2, 480, 7.f,7.f);
		utils::SetColor(Color4f{ 0.f, 0.f, 0.f,1.f });
		utils::FillEllipse(GetViewPort().width / 2, 480, 5.f, 5.f);
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	
	switch (e.keysym.sym)
	{
	case SDLK_LEFT:
		m_MainChar1State = MovementState::Left;
		m_MenueState = MenueState::Game;
		break;
	case SDLK_RIGHT:
		m_MainChar1State = MovementState::Right;
		m_MenueState = MenueState::Game;
		break;
	case SDLK_UP:
		m_MainChar1State = MovementState::Up;
		m_MenueState = MenueState::Game;
		break;
	case SDLK_DOWN:
		m_MainChar1State = MovementState::Down;
		m_MenueState = MenueState::Game;
		break;
	}

	switch (e.keysym.sym)
	{
	case SDLK_q:
		m_MainChar2State = MovementState::Left;
		m_MenueState = MenueState::Game;
		break;
	case SDLK_d:
		m_MainChar2State = MovementState::Right;
		m_MenueState = MenueState::Game;
		break;
	case SDLK_z:
		m_MainChar2State = MovementState::Up;
		m_MenueState = MenueState::Game;
		break;
	case SDLK_s:
		m_MainChar2State = MovementState::Down;
		m_MenueState = MenueState::Game;
		break;
	}

	if(m_MenueState == MenueState::Game)
	{
		switch (e.keysym.sym)
		{
		case SDLK_r:
		case SDLK_KP_0:
			if (!m_ExplosionOnCD)
			{
				m_ExplosionDuration = 0.f;
				m_MC1Explosion->Reset(m_MainChar1);
				m_MC2Explosion->Reset(m_MainChar2);
				explode = true;
				m_ExplosionOnCD = true;
				m_LastExplosionElapsed = 0.f;
			}
			break;
		case SDLK_SPACE:

			break;
		}
	}
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	if(m_MenueState==MenueState::Game||m_MenueState==MenueState::GameOver)
	{
		if (m_AliveState == AliveState::Both)
		{
			switch (e.keysym.sym)
			{
			case SDLK_a:
				if (m_MainChar2.radius > m_MinRadius)
				{
					m_MainChar1.radius += m_SizeChange;
					m_MainChar2.radius -= m_SizeChange;

					m_MainChar1Speed -= m_SpeedChange;
					m_MainChar2Speed += m_SpeedChange;
				};
				break;
			case SDLK_e:
				if (m_MainChar1.radius > m_MinRadius)
				{
					m_MainChar2.radius += m_SizeChange;
					m_MainChar1.radius -= m_SizeChange;

					m_MainChar1Speed += m_SpeedChange;
					m_MainChar2Speed -= m_SpeedChange;
				};
				break;

			}
		}

		switch (e.keysym.sym)
		{
		case SDLK_SPACE:
			m_MainChar1 = Circlef{ 50.f,300.f,20.f };
			m_MainChar2 = Circlef{ 50.f,200.f,20.f };
			m_MainChar1State = MovementState::Still;
			m_MainChar2State = MovementState::Still;
			m_MainChar1Speed = 100.f;
			m_MainChar2Speed = 100.f;
			m_CurrentHpGreen = 5;
			m_CurrentHpRed = 5;
			m_AliveState = AliveState::Both;
			explode = false;
			m_ExplosionDuration = 0.f;
			m_ExplosionOnCD = false;
			m_LastExplosionElapsed = 0.f;

			for (int resetCounter{}; resetCounter < m_NrMelees; ++resetCounter)
			{
				m_GreenMeleeEnemysVector[resetCounter]->Reset();
				m_RedMeleeEnemysVector[resetCounter]->Reset();

			}
			for (int resetCounter{}; resetCounter < m_NrRanged; ++resetCounter)
			{
				m_GreenRangedEnemysVector[resetCounter]->Reset();
				m_RedRangedEnemysVector[resetCounter]->Reset();

			}
			m_AliveState = AliveState::Both;
			m_MenueState = MenueState::Start;
			m_NrEnemiesDefeated = 0;
			m_EndMessageSend = false;
			break;

		}
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT );
}
