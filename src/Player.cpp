/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 * DirectX Bomns                                                             *
 * by Toxite                                                                 *
 *                                                                           *
 * This file is part of the source code to DXBomns.  Go to                   *
 * http://bomns.sourceforge.net/ to get the latest version of this source or *
 * the game.  I can be reached with questions about this source (or the      *
 * game) at Toxite@aol.com.                                                  *
 *                                                                           *
 * This source file was written with a tab size of 3.  I recommend you view  *
 * it as such, as it'll look a lot prettier.                                 *
 *                                                                           *
 * Copyright (c) 1998-2002 Charles Lindsay.  As DXBomns is open source and   *
 * freeware, you may use this file however you wish, so long as you don't    *
 * charge money for it, you keep all the original source together (ie don't  *
 * split this .zip up unless you modify it, in which case you can distribute *
 * it however you wish), and you credit me if you use this as a base for     *
 * anything else.  Thanks!                  --Toxite                         *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Player.cpp -- source for the player class, used by the world to control   //
// everything about the players.                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include "DXBomns.h"
#include "XNotDirect.h"
#include "Graphics.h"
#include "Level.h"
#include "World.h"
#include "Player.h"


extern DWORD g_dwCurrentTime; //from Game.cpp
extern DWORD g_dwPausedTime;  // .

extern INT g_anKills[4];      // .
extern INT g_anDeaths[4];     // .


INT   g_nMaxHealth = 0;       //global deals common between all players (I didn't feel like using static member deals)
INT   g_nHealthPowerup = 2;
INT   g_nMaxBomns = 0;
INT   g_nMaxRad = 0;
INT   g_nMinRad = 0;
INT   g_nMaxDam = 0;
INT   g_nMinDam = 0;
DWORD g_dwInvulTime = 0;


//creates and returns a player object, which starts at nX, nY
//returns NULL if failure
CPlayer * CreatePlayer(CGraphics * pGraphics, CImage * piSprites, INT nX, INT nY, const GAMOPTS * cpgo)
{
	CPlayer * pRet = new CPlayer();
	if(!pRet)
	{
		Error("new CPlayer", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(pGraphics, piSprites, nX, nY, cpgo))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}


//zero everything
CPlayer::CPlayer()
{
	m_nX = m_nY = 0;
	m_nHealth = 0;
	m_nBomns = 0;
	m_nBomnsOut = 0;
	m_nRad = 0;
	m_nDam = 0;
	m_bInvul = FALSE;
	m_dwNoInvulTime = 0;
	m_pCopyGraphics = NULL;
	m_piCopySprites = NULL;
}

//dealloc anything
CPlayer::~CPlayer()
{
	//don't delete copies
}

//initializes a player, copying the graphics system and sprites image
//spawns the player at nX, nY in game tiles, and sets the global vars
//to be consistent with cpgo
//returns whether it succeeds
BOOL CPlayer::Initialize(CGraphics * pGraphics, CImage * piSprites, INT nX, INT nY, const GAMOPTS * cpgo)
{
	g_nMaxHealth = cpgo->yMaxHealth;             //copy the options...come to think of it,
	g_nHealthPowerup = cpgo->yHealthPerPowerup;  //I don't know now why I didn't just make
	g_nMaxBomns = cpgo->yMaxBomns;               //a single GAMOPTS global in this file
	g_nMaxRad = cpgo->yMaxRadius;                //and just copy it, but I went THIS way,
	g_nMinRad = cpgo->yMinRadius;                //so I'll stick with it
	g_nMaxDam = cpgo->yMaxDamage;
	g_nMinDam = cpgo->yMinDamage;
	g_dwInvulTime = ((DWORD)cpgo->yInvulTime) * 1000;
	
	m_nX = nX;
	m_nY = nY;

	m_nHealth = cpgo->yStartHealth; //get all the opts
	m_nBomns = cpgo->yStartBomns;
	m_nBomnsOut = 0;
	m_nRad = cpgo->yStartRadius;
	m_nDam = cpgo->yStartDamage;

	m_bInvul = FALSE;
	m_dwNoInvulTime = 0;

	m_pCopyGraphics = pGraphics;
	m_piCopySprites = piSprites;
	return TRUE;
}

//nothing to reset, so it just returns TRUE
BOOL CPlayer::Reset()
{
	return TRUE;
}

//nothing to do... just returns TRUE
BOOL CPlayer::StartGame()
{
	return TRUE;
}

//nothing to do
VOID CPlayer::PauseGame()
{
}

//just adds to the invul timer
VOID CPlayer::UnpauseGame()
{
	m_dwNoInvulTime += g_dwCurrentTime - g_dwPausedTime;
}

//gets the position
VOID CPlayer::GetPosition(PINT pnX, PINT pnY)
{
	if(pnX)
		*pnX = m_nX;
	if(pnY)
		*pnY = m_nY;
}

//accessor, returns whether it's invul
BOOL CPlayer::IsInvul() const
{
	return m_bInvul;
}

//called by the world when a player's movement key is hit
//simply turns around and calls TryMove with its own internal vars
BOOL CPlayer::Move(INT nPlayer, MOVETYPE mt, CWorld * pwDest)
{
	if(!pwDest->TryMove(nPlayer,
                       (mt == MT_LEFT ? m_nX - 1 : (mt == MT_RIGHT ? m_nX + 1 : m_nX)),
                       (mt == MT_UP   ? m_nY - 1 : (mt == MT_DOWN  ? m_nY + 1 : m_nY)),
							  &m_nX, &m_nY))
		return FALSE;

	return TRUE;
}

//called by the world when the player hits his drop key
//it just calls TryDrop with its internal vars
BOOL CPlayer::Drop(INT nPlayer, CWorld * pwDest)
{
	if(!pwDest->TryDrop(nPlayer, m_nX, m_nY, m_nRad, m_nDam, &m_nBomns, &m_nBomnsOut))
		return FALSE;

	return TRUE;
}

//hurts a player, updates the score if necessary, and returns
//information about whether it's in dire need of health or is dead
//returns whether it succeeds
BOOL CPlayer::Hurt(INT nPlayer, INT nFrom, INT nDamage, PBOOL pbDeath, PBOOL pbBad)
{
	if(m_bInvul) //can't hurt me!
		return TRUE;

	m_nHealth -= nDamage; //subtract it
	if(m_nHealth == 1)
		*pbBad = TRUE; //they're hurt bad
	else if(m_nHealth <= 0)
	{
		m_nHealth = 0; //clip it at 0
		*pbDeath = TRUE; //they pied
		if(g_anDeaths[nPlayer - 1] < 99999) //update deaths if less than 99999
			g_anDeaths[nPlayer - 1]++;
		if(g_anKills[nFrom - 1] < 99999 && nPlayer != nFrom) //update kills if someone else killed 'im
			g_anKills[nFrom - 1]++;
	}

	return TRUE;
}

//returns the player's health
INT CPlayer::Health() const
{
	return m_nHealth;
}

//returns the player's number of bomns (I can't recall exactly where this is used, but whatever)
INT CPlayer::NumBomns() const
{
	return m_nBomns;
}

//gets all the player params... health, bomns, rad, and damage
VOID CPlayer::GetInfo(PINT pnHealth, PINT pnBomns, PINT pnRad, PINT pnDam) const
{
	if(pnHealth)
		*pnHealth = m_nHealth;
	if(pnBomns)
		*pnBomns = m_nBomns;
	if(pnRad)
		*pnRad = m_nRad;
	if(pnDam)
		*pnDam = m_nDam;
}

//called each frame by world, telling the player what it's sitting on
//this way, the player can collect powerups without having to move onto
//them... returns whether it picked up the item
BOOL CPlayer::Collect(BYTE yTile)
{
	switch(yTile)
	{
		case TILE_HEALTH: //health tile
			if(m_nHealth < g_nMaxHealth) //if they're hurt
			{
				m_nHealth += g_nHealthPowerup; //get it
				if(m_nHealth > g_nMaxHealth)
					m_nHealth = g_nMaxHealth;
				return TRUE;
			}
			break;

		case TILE_INVUL: //invul tile... always pick up and update time
			m_bInvul = TRUE;
			m_dwNoInvulTime = g_dwCurrentTime + g_dwInvulTime;
			return TRUE;

		case TILE_BOMN:
			if(m_nBomns < g_nMaxBomns) //pick up bomns if not at max
			{
				m_nBomns++;
				return TRUE;
			}
			break;

		case TILE_RAD_UP:
			if(m_nRad < g_nMaxRad) // you get the idea
			{
				m_nRad++;
				return TRUE;
			}
			break;

		case TILE_RAD_DOWN:
			if(m_nRad > g_nMinRad)
			{
				m_nRad--;
				return TRUE;
			}
			break;

		case TILE_DAM_UP:
			if(m_nDam < g_nMaxDam)
			{
				m_nDam++;
				return TRUE;
			}
			break;

		case TILE_DAM_DOWN:
			if(m_nDam > g_nMinDam)
			{
				m_nDam--;
				return TRUE;
			}
			break;
	}

	return FALSE;
}

//called by world when one of this players bomns explodes...
//simply decreases the number of bomns it has out
VOID CPlayer::BomnExplode()
{
	m_nBomnsOut--;
}

//each frame
//returns if it succeeds
BOOL CPlayer::Update(CWorld * pwDest)
{
	if(m_bInvul && g_dwCurrentTime >= m_dwNoInvulTime) //if it's time to be not invul
	{
		m_bInvul = FALSE; //don't be invul

		if(!pwDest->PlayerLoseInvul(m_nX)) //tell the world as such
			return FALSE;
	}

	return TRUE;
}

//render it... piDest is where to render to, nNum is which player number this is
//returns whether it succeeds
BOOL CPlayer::Render(CImage * piDest, INT nNum)
{
	if(m_nHealth <= 0)
		return TRUE;

	RECT rcDest;
	rcDest.left   = m_nX << 4;
	rcDest.top    = m_nY << 4;
	rcDest.right  = rcDest.left + 16;
	rcDest.bottom = rcDest.top + 16;
	RECT rcSource;
	rcSource.left   = nNum << 4;
	rcSource.top    = (m_bInvul ? 16 : 0);
	rcSource.right  = rcSource.left + 16;
	rcSource.bottom = rcSource.top + 16;

	if(!m_pCopyGraphics->Blit(piDest, &rcDest, 1, m_piCopySprites, &rcSource))
		return FALSE;

	return TRUE;
}