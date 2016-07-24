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
// Bomn.cpp -- classes et al. that control dropped bomns and explosions.     //
// Used by the world to control interactions between the bomns and all else. //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include <math.h>

#include "DXBomns.h"
#include "XNotDirect.h"
#include "Graphics.h"
#include "Sound.h"
#include "World.h"
#include "Bomn.h"


extern DWORD g_dwCurrentTime; //from Game.cpp
extern DWORD g_dwPausedTime;  // .


//creates a bomn system object... needs initialized world, etc.
//returns NULL if failure
CBomnSystem * CreateBomnSystem(CWorld * pWorld, CGraphics * pGraphics, CImage * piSprites, CSoundSystem * pSoundSystem)
{
	CBomnSystem * pRet = new CBomnSystem();
	if(!pRet)
	{
		Error("new CBomnSystem", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(pWorld, pGraphics, piSprites, pSoundSystem))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}


//just zero everything
CBomnSystem::CBomnSystem()
{
	m_pCopyWorld = NULL;
	m_pCopyGraphics = NULL;
	m_piCopySprites = NULL;
	m_pCopySoundSystem = NULL;
	m_piExplosionSprites = NULL;
	m_piExplosionScreen = NULL;
	m_psTick = NULL;
	m_pBomnHead = NULL;
	m_pExplosionHead = NULL;
}

//deallocate anything
CBomnSystem::~CBomnSystem()
{
	SAFEDELETE(m_piExplosionSprites)
	SAFEDELETE(m_piExplosionScreen)
	SAFEDELETE(m_psTick)
	SAFEDELETE(m_pBomnHead) //this actually deletes the whole list
	SAFEDELETE(m_pExplosionHead) //so does this
}

//initializes the bomn system
//needs initialized ... all parameters
//returns whether it succeeds
BOOL CBomnSystem::Initialize(CWorld * pWorld, CGraphics * pGraphics, CImage * piSprites, CSoundSystem * pSoundSystem)
{
	m_pCopyWorld = pWorld;

	m_pCopyGraphics = pGraphics;
	m_piCopySprites = piSprites;
	m_pCopySoundSystem = pSoundSystem;

	//create a 1024x768 image that will be filled with 12 16x16 blocks of 16x16 pixels each
	//of the explosion sprites in sprites.bmp.  Read on
	m_piExplosionSprites = m_pCopyGraphics->CreateImage(NULL, 1024, 768, FALSE, 0);
	if(!m_piExplosionSprites)
		return FALSE;

	for(INT i = 0; i < 10; i++) //there are only 10 explosion sprites in sprites.bmp, so for each one
	{
		RECT arcDest[256], arcSource[256]; //16x16=256 rectangles
		for(INT j = 0; j < 256; j++)
		{
			arcDest[j].left   = (i % 4) * 256 + (j % 16) * 16; //set up dest
			arcDest[j].top    = (i / 4) * 256 + (j / 16) * 16;
			arcDest[j].right  = arcDest[j].left + 16;
			arcDest[j].bottom = arcDest[j].top + 16;

			arcSource[j].left   =      (i == 9 ? 3 : i % 3) * 16; //set up source
			arcSource[j].top    = 48 + (i == 9 ? 2 : i / 3) * 16;
			arcSource[j].right  = arcSource[j].left + 16;
			arcSource[j].bottom = arcSource[j].top + 16;
		}
		//blit 256 times
		if(!m_pCopyGraphics->Blit(m_piExplosionSprites, arcDest, 256, m_piCopySprites, arcSource))
			return FALSE;
	}
	if(!m_piExplosionSprites->Backup()) //back it up, after drawing it, so that we never have to draw it again
		return FALSE;

	//create an image that holds ALL the explosions currently happening... the explosions draw to HERE,
	//and then THIS gets drawn to main screen
	m_piExplosionScreen = m_pCopyGraphics->CreateImage(NULL, SCR_WIDTH, SCR_HEIGHT - 48, TRUE, 0x0000);
	if(!m_piExplosionScreen)
		return FALSE;

	m_psTick = m_pCopySoundSystem->CreateSound(".\\sfx\\tick.wav"); //the bomns need tick.wav
	if(!m_psTick)
		return FALSE;

	return TRUE;
}

//just return true
BOOL CBomnSystem::StartGame()
{
	return TRUE;
}

//don't need to do anything
VOID CBomnSystem::PauseGame()
{
}

//for each bomn and explosion, add to its timer(s)
VOID CBomnSystem::UnpauseGame()
{
	for(BOMNNODE * pCur = m_pBomnHead; pCur; pCur = pCur->pNext)
	{
		pCur->dwExplodeTime += g_dwCurrentTime - g_dwPausedTime;
		pCur->dwNextBlinkTime += g_dwCurrentTime - g_dwPausedTime;
	}
	for(EXPLOSIONNODE * pDog = m_pExplosionHead; pDog; pDog = pDog->pNext)
		pDog->dwNextSwitchTime += g_dwCurrentTime - g_dwPausedTime;
}

//someone dropped a bomn, so add the bomn to this system
BOOL CBomnSystem::AddBomn(INT nX, INT nY, INT nOwn, INT nRad, INT nDam, INT nTimer)
{
	//just add it to the list
	BOMNNODE * pTemp = new BOMNNODE(nX, nY, nOwn, nRad, nDam, nTimer, m_pBomnHead);
	if(!pTemp)
	{
		Error("new BOMNNODE", "not enough memory");
		return FALSE;
	}
	m_pBomnHead = pTemp; //and set the head to the new one

	return TRUE;
}

//returns true if a bomn is currently waiting to go off at nX, nY (game tiles, of course)
//false otherwise
BOOL CBomnSystem::IsBomnAt(INT nX, INT nY)
{
	for(BOMNNODE * pCur = m_pBomnHead; pCur; pCur = pCur->pNext)
	{
		if(pCur->nX == nX && pCur->nY == nY)
			return TRUE;
	}

	return FALSE;
}

//reset any images herein
//returns whether it succeeds
BOOL CBomnSystem::Reset()
{
	if(!m_piExplosionSprites->Reset())
		return FALSE;
	if(!m_piExplosionScreen->Reset())
		return FALSE;

	if(!m_piExplosionSprites->RestoreFromBackup()) //we need to restore this one from backup
		return FALSE;

	for(EXPLOSIONNODE * pCur = m_pExplosionHead; pCur; pCur = pCur->pNext) //and redraw all the explosions
		pCur->bDraw = TRUE;

	return TRUE;
}

//each frame
//returns whether it succeeds
BOOL CBomnSystem::Update()
{
	BOMNNODE * pCur = m_pBomnHead; //start at the bomn head
	while(pCur)
	{
		BOMNNODE * pNext = pCur->pNext; //save the next pointer

		if(g_dwCurrentTime >= pCur->dwExplodeTime) //if it's supposed to explode
		{
			if(!Explode(pCur)) //tell that to the bomn system
				return FALSE;

			if(pCur == m_pBomnHead)       //if we're on the head
				m_pBomnHead = pCur->pNext; //set the head to the next pointer

			if(pCur->pNext) //if there's a next bomn in the list
				pCur->pNext->pPrev = pCur->pPrev; //set its prev to the other prev
			if(pCur->pPrev) //same for a prev bomn
				pCur->pPrev->pNext = pCur->pNext;

			pCur->pNext = pCur->pPrev = NULL; //isolate this one from the list
			SAFEDELETE(pCur) //then delete it
		}
		else if(g_dwCurrentTime >= pCur->dwNextBlinkTime) //if it's not supposed to blow up, but IS supposed to blink
		{
			pCur->bDraw = !pCur->bDraw; //reverse the draw flag
			if(pCur->bDraw) //if it's to be drawn
			{
				pCur->bShowTime = !pCur->bShowTime; //switch whether it shows the time or the generic sprite

				if(pCur->bShowTime) //if it's showing time
				{
					if(pCur->dwExplodeTime - g_dwCurrentTime <= 1000 && !pCur->bAboutCalled) //if it's only got one second left and we haven't called this yet
					{
						if(!m_pCopyWorld->BomnAboutToExplode(pCur->nX, pCur->nY, pCur->nRadius)) //tell the world it's about to go so it can play a sound if necessary
							return FALSE;
						pCur->bAboutCalled = TRUE; //set the flag so we don't call it again
					}

					if(!m_psTick->PanPlay(pCur->nX, FALSE)) //play that shit
						return FALSE;
				}
			}
			pCur->dwNextBlinkTime += 250; //and reset the timer
		}

		pCur = pNext; //go to the next one, from above
	}

	EXPLOSIONNODE * pDog = m_pExplosionHead; //start at the head
	while(pDog)
	{
		EXPLOSIONNODE * pNext = pDog->pNext; //save the next pointer

		if(g_dwCurrentTime >= pDog->dwNextSwitchTime) //if it's been a tenth of a second
		{
			pDog->bDraw = TRUE; //draw it again
			pDog->dwNextSwitchTime += 100; //reset counter
			pDog->nStage++; //it's one step farther along
		}

		if(pDog->bKill) //if it was set to be killed
		{
			if(pDog == m_pExplosionHead) //if it's the head
				m_pExplosionHead = pDog->pNext; //set the head to the next one

			if(pDog->pNext) //isolate this one from the list
				pDog->pNext->pPrev = pDog->pPrev;
			if(pDog->pPrev)
				pDog->pPrev->pNext = pDog->pNext;

			pDog->pNext = pDog->pPrev = NULL;
			SAFEDELETE(pDog) //delete it
		}

		pDog = pNext; //go to next pointer from above
	}

	return TRUE;
}

//renders the bomn system, drawing bomns and explosions
BOOL CBomnSystem::Render(CImage * piDest)
{
	for(BOMNNODE * pCur = m_pBomnHead; pCur; pCur = pCur->pNext) //for each bomn in the system
	{
		if(pCur->bDraw) //if we're to draw it
		{
			RECT rcDest;
			rcDest.left   = pCur->nX << 4; //set up rectangles
			rcDest.top    = pCur->nY << 4;
			rcDest.right  = rcDest.left + 16;
			rcDest.bottom = rcDest.top + 16;
			RECT rcSource;
			if(pCur->bShowTime)
			{
				INT nTime = (INT)ceil((pCur->dwExplodeTime - g_dwCurrentTime) / 1000.0f);
				rcSource.left = (8 - (nTime + 1) / 2) << 4;
				rcSource.top  = (3 + (nTime % 2)) << 4;
			}
			else
			{
				rcSource.left = 112;
				rcSource.top  = 32;
			}
			rcSource.right  = rcSource.left + 16;
			rcSource.bottom = rcSource.top + 16;

			//draw it
			if(!m_pCopyGraphics->Blit(piDest, &rcDest, 1, m_piCopySprites, &rcSource))
				return FALSE;
		}
	}

	for(EXPLOSIONNODE * pDog = m_pExplosionHead; pDog; pDog = pDog->pNext) //for each explosion
	{
		if(pDog->bDraw)
		{
			if(pDog->nRadius > 0) //if it's actually an explosion
			{
				RECT rcBounds; //holds the actual rectangle, in game tiles, that gets drawn
				rcBounds.left   = (pDog->nX - pDog->nRadius + 1 < 0 ? 0 : pDog->nX - pDog->nRadius + 1);
				rcBounds.top    = (pDog->nY - pDog->nRadius + 1 < 0 ? 0 : pDog->nY - pDog->nRadius + 1);
				rcBounds.right  = (pDog->nX + pDog->nRadius - 1 > LEVEL_WIDTH  - 1 ? LEVEL_WIDTH  - 1 : pDog->nX + pDog->nRadius - 1);
				rcBounds.bottom = (pDog->nY + pDog->nRadius - 1 > LEVEL_HEIGHT - 1 ? LEVEL_HEIGHT - 1 : pDog->nY + pDog->nRadius - 1);

				INT nTotalWidth  = rcBounds.right - rcBounds.left + 1; //actual width and height
				INT nTotalHeight = rcBounds.bottom - rcBounds.top + 1;

				for(INT i = 0; i < nTotalHeight; i += 16) //for each tile (drawn in blocks 16 tiles high)
				{
					INT nHeight = nTotalHeight - i; //move up i tiles from the total height
					if(nHeight > 16) //but do a max of 16
						nHeight = 16;

					for(INT j = 0; j < nTotalWidth; j += 16) //for each tile (drawn in blocks of 16 tiles wide)
					{
						INT nWidth = nTotalWidth - j; //move over j tiles from the total width
						if(nWidth > 16) //to a max of 16
							nWidth = 16;

						RECT rcDest;
						rcDest.left   = rcBounds.left + j;    //set up rects in terms of game tiles
						rcDest.top    = rcBounds.top + i;
						rcDest.right  = rcDest.left + nWidth;
						rcDest.bottom = rcDest.top + nHeight;

						RECT rcSource;
						rcSource.left = (pDog->nStage % 4) << 4;
						rcSource.top  = (pDog->nStage / 4) << 4;
						rcSource.right  = rcSource.left + nWidth;
						rcSource.bottom = rcSource.top + nHeight;

						rcDest.left   <<= 4; //translate to pixels
						rcDest.top    <<= 4;
						rcDest.right  <<= 4;
						rcDest.bottom <<= 4;
						rcSource.left   <<= 4;
						rcSource.top    <<= 4;
						rcSource.right  <<= 4;
						rcSource.bottom <<= 4;

						//draw it to the explosion screen
						if(!m_pCopyGraphics->Blit(m_piExplosionScreen, &rcDest, 1, m_piExplosionSprites, &rcSource))
							return FALSE;
					}
				}
			}

			pDog->bDraw = FALSE; //don't draw it again
			if(pDog->nStage >= 10) //and once it gets past stage 10 and draws itself there (effectively erasing it), kill it
				pDog->bKill = TRUE;
		}
	}

	//blit explosion screen to dest
	RECT rcDest = {0, 0, SCR_WIDTH, SCR_HEIGHT - 48};
	if(!m_pCopyGraphics->Blit(piDest, &rcDest, 1, m_piExplosionScreen, NULL))
		return FALSE;

	return TRUE;
}

//called when a bomn goes off... tells the world what's going on and
//sets up the explosion based on the bomn source
//returns whether it succeeds
BOOL CBomnSystem::Explode(const BOMNNODE * pSource)
{
	if(!m_pCopyWorld->Explode(pSource->nOwner, pSource->nX, pSource->nY, pSource->nRadius, pSource->nDamage))
		return FALSE;

	EXPLOSIONNODE * pTemp = new EXPLOSIONNODE(pSource->nX, pSource->nY, pSource->nRadius, m_pExplosionHead);
	if(!pTemp)
	{
		Error("new EXPLOSIONNODE", "not enough memory");
		return FALSE;
	}
	m_pExplosionHead = pTemp;

	return TRUE;
}