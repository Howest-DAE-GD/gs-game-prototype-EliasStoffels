#pragma once
#include "BaseGame.h"
#include "MeleeEnemy.h"
#include "Explosion.h"
#include "RangedEnemy.h"

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
	void UpdateMainChars(float elapsedSec);
	void UpdateMelees(float elapsedSec);
	void UpdateRanged(float elapsedSec);
	void DrawMainChars() const;
	void DrawEnemies() const;
	void DrawHp() const;
	
	//Characters
	Circlef m_MainChar1;
	Circlef m_MainChar2;

	float m_MainChar1Speed;
	float m_MainChar2Speed;

	float m_SizeChange;
	float m_MinRadius;

	float m_SpeedChange;

	const int MAX_HP;
	int m_CurrentHpRed;
	int m_CurrentHpGreen;

	bool explode;
	float m_ExplosionDuration;
	const float MAX_EXPLOSION_DURATION;
	bool m_ExplosionOnCD;
	float m_LastExplosionElapsed;
	const float EXPLOSION_CD;

	const float RANGED_SPAWN_TIME;
	float m_TotalElapsedSec;
	bool m_RangedSpawn;

	const float REVIVE_TIME;
	float m_CurrentReviveTime;
	
	int m_NrEnemiesDefeated;
	bool m_EndMessageSend;

	enum class MovementState
	{
		Up, Down, Left, Right, Still
	};

	MovementState m_MainChar1State;
	MovementState m_MainChar2State;

	enum class AliveState
	{
		Both, Red, Green, Neither
	};

	enum class MenueState
	{
		Start, Game, GameOver
	};

	MenueState m_MenueState;

	AliveState m_AliveState;

	// enemies
	int m_NrMelees;
	std::vector<MeleeEnemy*> m_RedMeleeEnemysVector;
	std::vector<MeleeEnemy*> m_GreenMeleeEnemysVector;

	int m_NrRanged;
	Explosion* m_MC1Explosion;
	Explosion* m_MC2Explosion;

	std::vector<RangedEnemy*> m_RedRangedEnemysVector;
	std::vector<RangedEnemy*> m_GreenRangedEnemysVector;
};