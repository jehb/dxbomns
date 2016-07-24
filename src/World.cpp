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
// World.cpp -- the class that holds the game engine together.  It holds all //
// the players and the level, and controls each of their interactions with   //
// each other, as well as explosions and bomns and stuff.  Read on.          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include "DXBomns.h"
#include "XNotDirect.h"
#include "Graphics.h"
#include "Sound.h"
#include "Font.h"
#include "Level.h"
#include "World.h"
#include "Player.h"
#include "Opts.h"


//creates a world object with an already-created graphics and soundsystem object
//returns NULL if failure
CWorld * CreateWorld(CGraphics * pGraphics, CSoundSystem * pSoundSystem)
{
	CWorld * pRet = new CWorld();
	if(!pRet)
	{
		Error("new CWorld", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(pGraphics, pSoundSystem))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}


//just zero everything
CWorld::CWorld()
{
	ZeroMemory(&m_goGameOpts, sizeof(m_goGameOpts));
	ZeroMemory(&m_kcConfig, sizeof(m_kcConfig));

	m_pLevel = NULL;
	for(INT i = 0; i < MAX_PLAYERS; i++)
		m_apPlayer[i] = NULL;
	m_nPlayers = 0;

	m_pBomnSystem = NULL;

	m_piSprites = NULL;

	for(i = 0; i < NUM_SOUNDS; i++)
		m_apsSound[i] = NULL;

	m_pCopyGraphics = NULL;
	m_pCopySoundSystem = NULL;
}

//dealloc anything
CWorld::~CWorld()
{
	SAFEDELETE(m_pLevel)
	for(INT i = 0; i < MAX_PLAYERS; i++)
		SAFEDELETE(m_apPlayer[i])

	SAFEDELETE(m_pBomnSystem)

	//don't delete the copies
	SAFEDELETE(m_piSprites)

	for(i = 0; i < NUM_SOUNDS; i++)
		SAFEDELETE(m_apsSound[i])
}

//initializes the world object
//returns whether it succeeded
BOOL CWorld::Initialize(CGraphics * pGraphics, CSoundSystem * pSoundSystem)
{
	GetGamOpts(&m_goGameOpts); //get the game and key options
	GetKeyConfig(&m_kcConfig);

	m_pCopyGraphics = pGraphics;
	m_pCopySoundSystem = pSoundSystem;

	m_piSprites = m_pCopyGraphics->CreateImage(".\\gfx\\sprites.bmp", 0, 0, FALSE, 0); //create the sprites bitmap
	if(!m_piSprites)
		return FALSE;

	for(INT i = 0; i < NUM_SOUNDS; i++)
	{
		m_apsSound[i] = m_pCopySoundSystem->CreateSound(g_cacszSoundFile[i]); //create all the sounds
		if(!m_apsSound[i])
			return FALSE;
	}

	m_pLevel = CreateLevel(LT_PLAY, m_pCopyGraphics, m_piSprites, m_pCopySoundSystem); //create the level
	if(!m_pLevel)
		return FALSE;

	if(!m_pLevel->LoadFromFile(".\\lev.dat")) //and load it from lev.dat
		return FALSE;

	m_nPlayers = (INT)m_goGameOpts.yPlayers; //get the number of players
	for(i = 0; i < m_nPlayers; i++)
	{
		BYTE yX, yY;
		m_pLevel->GetPlayerStart(i, &yX, &yY);
		m_apPlayer[i] = CreatePlayer(m_pCopyGraphics, m_piSprites, yX, yY, &m_goGameOpts); //initialize each one
		if(!m_apPlayer[i])
			return FALSE;
	}

	m_pBomnSystem = CreateBomnSystem(this, m_pCopyGraphics, m_piSprites, m_pCopySoundSystem); //initialize the bomn system
	if(!m_pBomnSystem)
		return FALSE;

	return TRUE;
}

//on game start
//returns whether it succeeded
BOOL CWorld::StartGame()
{
	if(!m_pLevel->StartGame()) //simply call StartGame for all the objects contained herein
		return FALSE;

	for(INT i = 0; i < m_nPlayers; i++)
	{
		if(!m_apPlayer[i]->StartGame())
			return FALSE;
	}

	if(!m_pBomnSystem->StartGame())
		return FALSE;

	return TRUE;
}

//on game pause
VOID CWorld::PauseGame()
{
	m_pLevel->PauseGame();  //call pause for all objects

	for(INT i = 0; i < m_nPlayers; i++)
		m_apPlayer[i]->PauseGame();

	m_pBomnSystem->PauseGame();
}

//on game unpause
VOID CWorld::UnpauseGame()
{
	m_pLevel->UnpauseGame(); //call unpause for all objects

	for(INT i = 0; i < m_nPlayers; i++)
		m_apPlayer[i]->UnpauseGame();

	m_pBomnSystem->UnpauseGame();
}

//called by Game.cpp whenever someone hits a key
//this processes the keystroke, sending it to whichever player it belongs to
//returns whether it succeeded
BOOL CWorld::KeyHit(BYTE yKey)
{
	for(INT i = 0; i < m_nPlayers; i++) //for each player
	{
		if(m_apPlayer[i]->Health() <= 0) //if it's alive
			continue;

		//this turns the m_kcConfig into a stream of bytes, to make it easier to retrieve the keys
		//for different players.  Then, it calls player->move for any key that matches
		if(yKey == ((PBYTE)&m_kcConfig)[i * 5 + 0] && !m_apPlayer[i]->Move(i + 1, MT_UP, this))
			return FALSE;
		else if(yKey == ((PBYTE)&m_kcConfig)[i * 5 + 1] && !m_apPlayer[i]->Move(i + 1, MT_DOWN, this))
			return FALSE;
		else if(yKey == ((PBYTE)&m_kcConfig)[i * 5 + 2] && !m_apPlayer[i]->Move(i + 1, MT_LEFT, this))
			return FALSE;
		else if(yKey == ((PBYTE)&m_kcConfig)[i * 5 + 3] && !m_apPlayer[i]->Move(i + 1, MT_RIGHT, this))
			return FALSE;
		else if(yKey == ((PBYTE)&m_kcConfig)[i * 5 + 4] && !m_apPlayer[i]->Drop(i + 1, this))
			return FALSE;
	}

	return TRUE;
}

//called by Player.cpp whenever KeyHit tells the players they're trying to move
//this way, the WORLD tells the players where they're moving, instead of the players
//telling the world (makes clipping et al. easier).  nPlayer is which player's trying
//to move... nTryX and nTryY are the x,y spot it's trying to move to, pnOutX and pnOutY
//are the player's x and y coords, that this function updates.
//returns whether it succeeded
BOOL CWorld::TryMove(INT nPlayer, INT nTryX, INT nTryY, PINT pnOutX, PINT pnOutY)
{
	if(nTryX < 0 || nTryX >= LEVEL_WIDTH || nTryY < 0 || nTryY >= LEVEL_HEIGHT) //if the spot is out of the level
	{  //don't update pnOutX, pnOutY
		if(!m_apsSound[SND_NOGO]->PanPlay(nTryX, FALSE)) //play the no go sound
			return FALSE;

		return TRUE;
	}

	for(INT i = 0; i < m_nPlayers; i++) //check for each player
	{
		if(i == nPlayer - 1 || m_apPlayer[i]->Health() <= 0) //if it's alive, and not this player
			continue;

		INT nX, nY;
		m_apPlayer[i]->GetPosition(&nX, &nY);
		if(nTryX == nX && nTryY == nY) //if they're smacking together
		{
			BOOL bDeath = FALSE, bHurtBad = FALSE; //hurt 'im
			if(!m_apPlayer[i]->Hurt(i + 1, nPlayer, m_goGameOpts.yPunchDamage, &bDeath, &bHurtBad))
				return FALSE;
			if(!m_apsSound[SND_PUNCH]->PanPlay(nTryX, FALSE))
				return FALSE;
			if(bDeath && !m_apsSound[SND_DEATH]->PanPlay(nX, FALSE)) //play the appropriate sounds
				return FALSE;
			else if(bHurtBad && !m_apsSound[SND_NEEDHEALTH]->PanPlay(nX, FALSE))
				return FALSE;

			return TRUE;
		}
	}

	BYTE yTile = m_pLevel->GetTile(MAKEOFFSET(nTryX, nTryY)); //get the tile of the new position
	switch(yTile)
	{
		case TILE_WALL_NORMAL:
			if(m_apPlayer[nPlayer - 1]->IsInvul())
			{
				if(!m_pLevel->Destroy(MAKEOFFSET(nTryX, nTryY), FALSE, FALSE)) //destry it if they're invul
					return FALSE;
			}
			else
			{
				if(!m_apsSound[SND_NOGO]->PanPlay(nTryX, FALSE)) //no go if not invul
					return FALSE;
			}
			break;

		case TILE_WALL_INDEST:
			if(!m_apsSound[SND_NOGO]->PanPlay(nTryX, FALSE)) //no go
				return FALSE;
			break;

		case TILE_WARP:
		{
			INT anOtherOffset[3];
			INT j = 0;
			for(INT i = 0; i < m_nPlayers; i++) //compile a list of the other players' positions
			{
				if(i == nPlayer - 1 || m_apPlayer[i]->Health() <= 0)
					continue;

				INT nX, nY;
				m_apPlayer[i]->GetPosition(&nX, &nY);
				anOtherOffset[j++] = MAKEOFFSET(nX, nY);
			}
			for(; j < 3; j++)
				anOtherOffset[j] = anOtherOffset[j < 1 ? 0 : j - 1]; //make sure none of the list elements are gibberish

			UINT unDest = m_pLevel->GetSpawnDestination(anOtherOffset[0], anOtherOffset[1], anOtherOffset[2]); //tell level to get a warp dest, avoiding other players' positions
			*pnOutX = XFROMOFFSET(unDest);
			*pnOutY = YFROMOFFSET(unDest);

			if(!m_apsSound[SND_WARP]->PanPlay(nTryX, FALSE)) //warp
				return FALSE;
			break;
		}

		default:
			*pnOutX = nTryX; //OTHERWISE, just go there
			*pnOutY = nTryY;
			if(!m_apsSound[SND_STEP]->PanPlay(nTryX, FALSE))
				return FALSE;
			break;
	}

	return TRUE;
}

//called when KeyHit tells players they're trying to drop a bomn
//this way, the WORLD tells players when they can drop or not
//nPlayer is the player who's trying to drop, nX and nY are where,
//nRad and nDam are params for the bomn, pnBomns is a pointer to how
//many bomns the player has, and pnBomnsOut is how many bomns the player has
//out.  Returns whether it succeeds
BOOL CWorld::TryDrop(INT nPlayer, INT nX, INT nY, INT nRad, INT nDam, PINT pnBomns, PINT pnBomnsOut)
{
	//if the player's got some bomns and there's not already a bomn there and they don't have too many bomns out
	if(*pnBomns && !m_pBomnSystem->IsBomnAt(nX, nY) && *pnBomnsOut < (INT)m_goGameOpts.yMaxBomnsOut)
	{
		if(!m_pBomnSystem->AddBomn(nX, nY, nPlayer, nRad, nDam, (INT)m_goGameOpts.yBomnTimer))
			return FALSE; //add a bomn to the bomnsystem

		if(!m_apsSound[SND_DROP]->PanPlay(nX, FALSE)) //drop
			return FALSE;

		(*pnBomns)--;
		(*pnBomnsOut)++;
	}
	else
	{
		if(!m_apsSound[SND_NODROP]->PanPlay(nX, FALSE)) //no drop
			return FALSE;
	}

	return TRUE;
}

//called by the player when it loses invul, to play the sound
BOOL CWorld::PlayerLoseInvul(INT nX)
{
	if(!m_apsSound[SND_VUL]->PanPlay(nX, FALSE))
		return FALSE;

	return TRUE;
}

//called by BomnSystem when a bomn has one second left... so that
//the world can play a sound if a player's in the blast radius and
//is about to get hurt
//returns whether it succeeded
BOOL CWorld::BomnAboutToExplode(INT nX, INT nY, INT nRad)
{
	for(INT i = 0; i < m_nPlayers; i++) //for each player
	{
		if(m_apPlayer[i]->Health() <= 0) //for each LIVING player
			continue;

		INT nPlayerX, nPlayerY;
		m_apPlayer[i]->GetPosition(&nPlayerX, &nPlayerY);

		if(nPlayerX > nX - nRad && nPlayerX < nX + nRad && nPlayerY > nY - nRad && nPlayerY < nY + nRad)
		{
			if(!m_apsSound[SND_UHOH]->PanPlay(nPlayerX, FALSE)) //if the player's in the radius, play the sound
				return FALSE;
		}
	}

	return TRUE;
}

//called when a bomn explodes... eliminates game tiles 'n' such
//returns whether it succeeds
BOOL CWorld::Explode(INT nOwner, INT nX, INT nY, INT nRad, INT nDam)
{
	m_apPlayer[nOwner - 1]->BomnExplode();

	for(INT i = nY - nRad + 1; i < nY + nRad; i++) //for each tile in the explosion
	{
		for(INT j = nX - nRad + 1; j < nX + nRad; j++)
		{
			if(i >= 0 && i < LEVEL_HEIGHT && j >= 0 && j < LEVEL_WIDTH) //if it's on the screen
			{
				if(!m_pLevel->Destroy(MAKEOFFSET(j, i), TRUE, m_goGameOpts.yPersistentPowerups)) //destroy it
					return FALSE;
			}
		}
	}

	if(!m_apsSound[SND_EXPLODE]->Play(FALSE)) //play the sound
		return FALSE;

	for(i = 0; i < m_nPlayers; i++)  //for each player
	{
		if(m_apPlayer[i]->Health() <= 0) //if it's alive
			continue;

		INT nPlayerX, nPlayerY;
		m_apPlayer[i]->GetPosition(&nPlayerX, &nPlayerY);

		if(nPlayerX > nX - nRad && nPlayerX < nX + nRad && nPlayerY > nY - nRad && nPlayerY < nY + nRad)
		{
			BOOL bDeath = FALSE, bBad = FALSE;
			if(!m_apPlayer[i]->Hurt(i + 1, nOwner, nDam, &bDeath, &bBad)) //hurt it if it's in the blast radius
				return FALSE;
			if(bDeath && !m_apsSound[SND_DEATH]->PanPlay(nPlayerX, FALSE))
				return FALSE;
			else if(bBad && !m_apsSound[SND_NEEDHEALTH]->PanPlay(nPlayerX, FALSE))
				return FALSE;
		}
	}
	return TRUE;
}

//calculates whether someone has won the game
//returns: 0 if nobody has won yet, 1-4 indicate which player has won,
//and 5 if it's a tie
INT CWorld::Winner()
{
	if(m_nPlayers == 1 || m_nPlayers == 0) //only one or zero players, so nobody CAN win
		return 0;

	INT nAlive = 0; //number alive
	INT nHe = 0;    //the last one found alive
	for(INT i = 0; i < m_nPlayers; i++) //check the players
	{
		if(m_apPlayer[i]->Health() > 0)
		{
			nAlive++;
			nHe = i + 1;
		}
	}

	if(nAlive == 0)     //if none are alive, it's a tie
		return 5;
	else if(nAlive > 1) //if more than one is alive, it's still in progress
		return 0;
	else                //if there's only one alive
		return nHe;      //return 'im
}

//gets a player's health, bomns, rad, and damage
VOID CWorld::GetPlayerInfo(INT nPlayer, PINT pnHealth, PINT pnBomns, PINT pnRad, PINT pnDam)
{
	if(nPlayer >= 1 && nPlayer <= m_nPlayers)
		m_apPlayer[nPlayer - 1]->GetInfo(pnHealth, pnBomns, pnRad, pnDam);
}

//resets any images contained herein, and calls reset for all the objects
BOOL CWorld::Reset()
{
	if(!m_piSprites->Reset())
		return FALSE;

	if(!m_pLevel->Reset())
		return FALSE;

	for(INT i = 0; i < m_nPlayers; i++)
	{
		if(!m_apPlayer[i]->Reset())
			return FALSE;
	}

	if(!m_pBomnSystem->Reset())
		return FALSE;

	return TRUE;
}

//each frame
//returns whether it succeeded
BOOL CWorld::Update()
{
	if(!m_pLevel->Update())
		return FALSE;

	if(!m_pBomnSystem->Update())
		return FALSE;
	
	for(INT i = 0; i < m_nPlayers; i++) //for each living player
	{
		if(m_apPlayer[i]->Health() <= 0)
			continue;

		INT nX, nY;
		m_apPlayer[i]->GetPosition(&nX, &nY);
		BYTE yTile = m_pLevel->GetTile(MAKEOFFSET(nX, nY)); //get the tile that it's sitting on
		if(m_apPlayer[i]->Collect(yTile)) //let the player collect it if it wants
		{
			m_pLevel->Destroy(MAKEOFFSET(nX, nY), FALSE, FALSE); //get rid of it

			switch(yTile) //play the appropriate sound
			{
				case TILE_HEALTH:
					if(!m_apsSound[SND_HEALTH]->PanPlay(nX, FALSE))
						return FALSE;
					break;

				case TILE_INVUL:
					if(!m_apsSound[SND_INVUL]->PanPlay(nX, FALSE))
						return FALSE;
					break;

				default:
					if(!m_apsSound[SND_COLLECT]->PanPlay(nX, FALSE))
						return FALSE;
					break;
			}
		}

		if(!m_apPlayer[i]->Update(this))
			return FALSE;
	}

	return TRUE;
}

//render this piece
//simply calls render for each of its children
//only renders the HUD if bRenderHud is true
//piDest is the destination, pFont is a valid font object
//returns whether it succeeds
BOOL CWorld::Render(CImage * piDest, BOOL bRenderHUD, CFont * pFont)
{
	if(!m_pLevel->Render(piDest))
		return FALSE;

	for(INT i = 0; i < m_nPlayers; i++)
	{
		if(!m_apPlayer[i]->Render(piDest, i + 1)) //render the player
			return FALSE;

		if(bRenderHUD) //render the HUD
		{
			RECT rcDest;
			rcDest.left   = 230 * i + (i < 2 ? 4 : 108);
			rcDest.top    = 724;
			rcDest.right  = rcDest.left + 16;
			rcDest.bottom = rcDest.top + 16;
			RECT rcSource;
			rcSource.left   = (i + 1) << 4;
			rcSource.top    = (m_apPlayer[i]->Health() <= 0 ? 2 : (m_apPlayer[i]->IsInvul() ? 1 : 0)) << 4;
			rcSource.right  = rcSource.left + 16;
			rcSource.bottom = rcSource.top + 16;

			if(!m_pCopyGraphics->Blit(piDest, &rcDest, 1, m_piSprites, &rcSource)) //render the player's status
				return FALSE;

			rcSource.left   = (i + 4) << 4;
			rcSource.top    = 80;
			rcSource.right  = rcSource.left + 16;
			rcSource.bottom = rcSource.top + 16;
			for(INT j = 0; j < m_apPlayer[i]->Health(); j++) //render each of the player's hearts
			{
				rcDest.left   = 230 * i + (i < 2 ? 27 : 131) + (j << 4);
				rcDest.right  = rcDest.left + 16;

				if(!m_pCopyGraphics->Blit(piDest, &rcDest, 1, m_piSprites, &rcSource))
					return FALSE;
			}
			
			if(m_apPlayer[i]->Health() > 0) //render how many bomns the player has
			{
				if(pFont && !pFont->Scribe(piDest, 230 * i + (i < 2 ? 194 : 298), 724, "%2d", m_apPlayer[i]->NumBomns()))
					return FALSE;
			}
		}
	}

	if(!m_pBomnSystem->Render(piDest))  //render the bomns
		return FALSE;

	return TRUE;
}