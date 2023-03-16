#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : 
	m_player(400, 540, 0, 0, 0)	
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	//player gravity
	if (m_state == AIRBORNE)
	{
		m_player.Accelerate(0, -50);
	}

	// Run and Stand
	if (m_state == STANDING || m_state == RUNNING)
	{
		m_player.SetVelocity(0, 0);
		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
		{
			m_player.Accelerate(-300, 0);
			if (m_state != RUNNING || m_side != LEFT)
				m_player.SetAnimation("run_left");
			m_state = RUNNING;
			m_side = LEFT;
		}
		else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
		{
			m_player.Accelerate(300, 0);
			if (m_state != RUNNING || m_side != RIGHT)
				m_player.SetAnimation("run_right");
			m_state = RUNNING;
			m_side = RIGHT;
		}
		else
		{
			if (m_state == RUNNING)
				m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
			m_state = STANDING;
		}
	}

	// Pre-Update Position
	CVector v0 = m_player.GetPos();

	// Updates
	for (CSprite *pSprite : m_sprites)
		pSprite->Update(t);
	m_player.Update(t);

	int height = m_player.GetHeight() / 2 - 1;
	int width = m_player.GetWidth() / 2 - 1;
	bool touching_platform = false;
	for (CSprite* pSprite : m_sprites)
	{
		if (m_player.HitTest(pSprite, 0))
		{
			if ((string)pSprite->GetProperty("tag") == "platform")
			{
				if (v0.m_y >= pSprite->GetTop() + height)
				{
					touching_platform = true;
					m_player.SetVelocity(0, 0);
					m_player.SetY(pSprite->GetTop() + height);
				}
				else if (v0.m_x >= pSprite->GetRight() + width)
				{
					touching_platform = true;
					m_player.SetVelocity(0, 0);
					m_player.SetX(pSprite->GetRight() + width);
				}
			}
		}
	}
	//control state transitions
	if (m_state == AIRBORNE && touching_platform)
	{
		//landed
		m_state = STANDING;
		m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
	}
	if (m_state != AIRBORNE && !touching_platform)
	{
		m_state = AIRBORNE;
		m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
	}
}

void CMyGame::OnDraw(CGraphics* g)
{
	for (CSprite *pSprite : m_sprites)
		pSprite->Draw(g);
	m_player.Draw(g);
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	m_player.LoadImage("player.png", "stand_right", 11, 6, 0, 0, CColor::White());
	m_player.LoadImage("player.png", "stand_left", 11, 6, 0, 1, CColor::White());
	m_player.AddImage("player.png", "run_right",     11, 6, 0, 0, 10, 0, CColor::White());
	m_player.AddImage("player.png", "run_left",      11, 6, 0, 1, 10, 1, CColor::White());
	m_player.LoadImage("player.png", "jump_right",   11, 6, 3, 2, CColor::White());
	m_player.LoadImage("player.png", "jump_left",    11, 6, 3, 3, CColor::White());
	m_player.LoadImage("player.png", "crouch_right", 11, 6, 2, 4, CColor::White());
	m_player.LoadImage("player.png", "crouch_left",  11, 6, 2, 5, CColor::White());
	m_player.LoadImage("player.png", "hang",		 11, 6, 10, 2, CColor::White());
	m_player.AddImage("player.png", "climb",		 11, 6, 9, 2, 10, 2, CColor::White());
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
	// Clean up first
	for (CSprite *pSprite : m_sprites)
		delete pSprite;
	m_sprites.clear();

	CSprite *pSprite;

	switch (nLevel)
	{
	case 1:// build Level 1 sprites

		// spawn the player
		m_player.SetPos(20, 530);
		m_player.SetImage("stand_right");

		// spawn all other sprites here ...

		// platforms
		pSprite = new CSpriteRect(400, 10, 800, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		// Enemies
		pSprite = new CSprite(20, 30, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(100, 0);
		pSprite->SetOmega(3.82 * 100);
		m_sprites.push_back(pSprite);


		break;

	case 2:// Level 2 (EMPTY)

		break;
	}

	// any initialisation common to all levels
	m_player.SetImage("stand_right");
	m_player.SetVelocity(0, 0);
	m_state = AIRBORNE;
	m_side = RIGHT;
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
	if ((sym == SDLK_w || sym == SDLK_UP) && (m_state == STANDING || m_state == RUNNING))
	{
		m_player.Accelerate(0, 800);

		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
		{
			m_player.Accelerate(-300, 0);
		}
		else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
		{
			m_player.Accelerate(300, 0);
		}

		m_state = AIRBORNE;
		m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
