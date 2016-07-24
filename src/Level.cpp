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
// Level.cpp -- the level class that is used by the game engine, the editor  //
// dialog, and the map editor itself.  It rules.                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include <stdlib.h>

#include "DXBomns.h"
#include "XNotDirect.h"
#include "Graphics.h"
#include "Sound.h"
#include "Opts.h"
#include "Level.h"


extern DWORD g_dwCurrentTime; //from Game.cpp
extern DWORD g_dwPausedTime;


//creates a level object... specify LT_GENERATEANDDUMP when not in graphics mode
//(the pGraphics, piSprites, and pSoundSystem can be NULL)... for LT_EDIT and LT_PLAY
//you need valid objects.  returns null in case of failure
CLevel * CreateLevel(LEVELTYPE ltType, CGraphics * pGraphics, CImage * piSprites, CSoundSystem * pSoundSystem)
{
	CLevel * pRet = new CLevel();
	if(!pRet)
	{
		Error("new CLevel", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(ltType, pGraphics, piSprites, pSoundSystem))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}

//just zero everything
CLevel::CLevel()
{
	m_ltType = LT_GENERATEANDDUMP;
	ZeroMemory(m_ayTile, sizeof(m_ayTile));
	ZeroMemory(m_ayPlayerStart, sizeof(m_ayPlayerStart));

	ZeroMemory(m_ayDraw, sizeof(m_ayDraw));

	ZeroMemory(&m_loLevOpts, sizeof(m_loLevOpts));
	m_dwNextSpawnTime = 0;

	m_piScreen = NULL;

	m_psCrumble = NULL;
	m_psSpawn = NULL;

	m_pCopyGraphics = NULL;
	m_piCopySprites = NULL;
	m_pCopySoundSystem = NULL;
}

//deallocate anything
CLevel::~CLevel()
{
	SAFEDELETE(m_piScreen)

	SAFEDELETE(m_psSpawn)
	SAFEDELETE(m_psCrumble)
	//don't delete copies, they're just copies
}

//initializes a level... see the note above CreateLevel
//returns whether it succeeds
BOOL CLevel::Initialize(LEVELTYPE ltType, CGraphics * pGraphics, CImage * piSprites, CSoundSystem * pSoundSystem)
{
	m_ltType = ltType;

	m_pCopyGraphics = pGraphics;
	m_piCopySprites = piSprites;
	m_pCopySoundSystem = pSoundSystem;

	Clear(); //set the level to 0, again

	if(m_ltType == LT_PLAY) //only get options if we're playing
	{
		GetLevOpts(&m_loLevOpts);
		m_dwNextSpawnTime = 0;
	}
	
	if(m_ltType != LT_GENERATEANDDUMP) //don't do it if not in graphics mode
	{
		m_piScreen = m_pCopyGraphics->CreateImage(NULL, SCR_WIDTH, SCR_HEIGHT - 48, FALSE, 0);
		if(!m_piScreen)
			return FALSE;
	}

	if(m_ltType == LT_PLAY) //only do if playing
	{
		m_psCrumble = m_pCopySoundSystem->CreateSound(".\\sfx\\crumble.wav");
		if(!m_psCrumble)
			return FALSE;

		m_psSpawn = m_pCopySoundSystem->CreateSound(".\\sfx\\spawn.wav");
		if(!m_psSpawn)
			return FALSE;
	}

	return TRUE;
}

//zeros a level and resets the player start points
VOID CLevel::Clear()
{
	ZeroMemory(m_ayTile, sizeof(m_ayTile));
	m_ayPlayerStart[0] = LEVEL_WIDTH  * 1 / 3;
	m_ayPlayerStart[1] = LEVEL_HEIGHT * 1 / 3;
	m_ayPlayerStart[2] = LEVEL_WIDTH  * 2 / 3;
	m_ayPlayerStart[3] = LEVEL_HEIGHT * 1 / 3;
	m_ayPlayerStart[4] = LEVEL_WIDTH  * 1 / 3;
	m_ayPlayerStart[5] = LEVEL_HEIGHT * 2 / 3;
	m_ayPlayerStart[6] = LEVEL_WIDTH  * 2 / 3;
	m_ayPlayerStart[7] = LEVEL_HEIGHT * 2 / 3;

	DrawAll(); //all tiles need to be redrawn
}

//generates additional tiles (call Clear() first to generate from scratch) into a level,
//based on the params in genopts.dat
//returns whether it succeeds
BOOL CLevel::Generate()
{
	GENOPTS go;
	if(!GetGenOpts(&go)) //read in the opts
		return FALSE;

	INT nTotal = 0;
	for(INT i = 0; i < LEVEL_SIZE; i++)
	{
		if(m_ayTile[i] != TILE_BLANK)
			nTotal++;
	}
	if(go.wHealths > 2880 - nTotal) //make sure we don't go over the magic 2880
		go.wHealths = 2880 - nTotal;
	nTotal += go.wHealths;
	if(go.wBomns > 2880 - nTotal)
		go.wBomns = 2880 - nTotal;
	nTotal += go.wBomns;
	if(go.wInvuls > 2880 - nTotal)
		go.wInvuls = 2880 - nTotal;
	nTotal += go.wInvuls;
	if(go.wRadUps > 2880 - nTotal)
		go.wRadUps = 2880 - nTotal;
	nTotal += go.wRadUps;
	if(go.wRadDowns > 2880 - nTotal)
		go.wRadDowns = 2880 - nTotal;
	nTotal += go.wRadDowns;
	if(go.wDamUps > 2880 - nTotal)
		go.wDamUps = 2880 - nTotal;
	nTotal += go.wDamUps;
	if(go.wDamDowns > 2880 - nTotal)
		go.wDamDowns = 2880 - nTotal;
	nTotal += go.wDamDowns;
	if(go.wNormalWalls > 2880 - nTotal)
		go.wNormalWalls = 2880 - nTotal;
	nTotal += go.wNormalWalls;
	if(go.wIndestWalls > 2880 - nTotal)
		go.wIndestWalls = 2880 - nTotal;
	nTotal += go.wIndestWalls;
	if(go.wWarps > 2880 - nTotal)
		go.wWarps = 2880 - nTotal;
	nTotal += go.wWarps;           //keep EVERYTHING under 2880

	nTotal = 0;
	for(i = 0; i < LEVEL_SIZE; i++)
	{
		if(m_ayTile[i] == TILE_WALL_NORMAL || m_ayTile[i] == TILE_WALL_INDEST || m_ayTile[i] == TILE_WARP)
			nTotal++;
	}
	if(nTotal > 2876) //see if we've got more than 2876 tiles (remember, we need 4 tiles free from walls and warps for player starts)
		return FALSE;

	if(go.wNormalWalls > 2876 - nTotal)
		go.wNormalWalls = 2876 - nTotal;
	nTotal += go.wNormalWalls;
	if(go.wIndestWalls > 2876 - nTotal)
		go.wIndestWalls = 2876 - nTotal;
	nTotal += go.wIndestWalls;
	if(go.wWarps > 2876 - nTotal)
		go.wWarps = 2876 - nTotal;
	nTotal += go.wWarps;     //go back and make sure we won't total more than 2876 on walls + warps
	
	//go is now valid

	if(go.dwSeed)
		srand(go.dwSeed); //use the seed if it's specified

	for(i = 0; i < go.wHealths; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_HEALTH; //spawn all the deals
	for(i = 0; i < go.wBomns; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_BOMN;
	for(i = 0; i < go.wInvuls; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_INVUL;
	for(i = 0; i < go.wRadUps; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_RAD_UP;
	for(i = 0; i < go.wRadDowns; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_RAD_DOWN;
	for(i = 0; i < go.wDamUps; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_DAM_UP;
	for(i = 0; i < go.wDamDowns; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_DAM_DOWN;
	for(i = 0; i < go.wNormalWalls; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_WALL_NORMAL;
	for(i = 0; i < go.wIndestWalls; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_WALL_INDEST;
	for(i = 0; i < go.wWarps; i++)
		m_ayTile[GetRandomEmptyTile(FALSE)] = TILE_WARP;

	FillMemory(m_ayPlayerStart, sizeof(m_ayPlayerStart), 255); //reset all player starts
	for(i = 0; i < 4; i++)
	{
		UINT unPlayerOffset = GetRandomEmptyTile(TRUE); //and then regenerate them
		m_ayPlayerStart[i * 2 + 0] = XFROMOFFSET(unPlayerOffset);
		m_ayPlayerStart[i * 2 + 1] = YFROMOFFSET(unPlayerOffset);

	}
	
	if(go.dwSeed) //re-randomize so that the same level won't play the same way
		srand(GetTickCount());

	DrawAll(); //everything needs to be drawn

	return TRUE;
}

//loads a level from the flie specified by cszFile
//always returns TRUE, but theoretically, it COULD return false for an error...
BOOL CLevel::LoadFromFile(PCSTR cszFile)
{
	//open the file
	HANDLE hFile = CreateFile(cszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
		return TRUE;//no error here... (in other words, if this fucks up, don't do shit about it)

	//read it in
	DWORD dwSize = sizeof(m_ayTile);
	if(!ReadFile(hFile, m_ayTile, sizeof(m_ayTile), &dwSize, NULL) || dwSize != sizeof(m_ayTile))
	{
		CloseHandle(hFile);
		return TRUE;
	}

	//read in player starts
	dwSize = sizeof(m_ayPlayerStart);
	if(!ReadFile(hFile, m_ayPlayerStart, sizeof(m_ayPlayerStart), &dwSize, NULL) || dwSize != sizeof(m_ayPlayerStart))
	{
		CloseHandle(hFile);
		return TRUE;
	}
	CloseHandle(hFile);

	DrawAll(); //everything needs to be redrawn

	return TRUE;
}

//saves the contents of the level to the file specified by cszFile
//always returns TRUE
BOOL CLevel::DumpToFile(PCSTR cszFile)
{
	if(!IsValid())//only save if it's valid
		return TRUE;

	//open
	HANDLE hFile = CreateFile(cszFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
		return TRUE;

	//write level
	DWORD dwSize = sizeof(m_ayTile);
	if(!WriteFile(hFile, m_ayTile, sizeof(m_ayTile), &dwSize, NULL) || dwSize != sizeof(m_ayTile))
	{
		CloseHandle(hFile);
		return TRUE;
	}

	//write player starts
	dwSize = sizeof(m_ayPlayerStart);
	if(!WriteFile(hFile, m_ayPlayerStart, sizeof(m_ayPlayerStart), &dwSize, NULL) || dwSize != sizeof(m_ayPlayerStart))
	{
		CloseHandle(hFile);
		return TRUE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//godDAMN this function is long.  It creates preview.dat, which will hold a 4-bit bitmap
//of the preview of the level, based on the tiles from gfx\preview.bmp.
//it returns whether it succeeds in its task
BOOL CLevel::GeneratePreview()
{
	//open the bitmap that contains our tiles (and must be 4 bits!)
	HBITMAP hBmpTiles = (HBITMAP)LoadImage(NULL, ".\\gfx\\preview.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if(!hBmpTiles)
	{
		Error("LoadImage", "unable to load the preview tiles bitmap (gfx\\preview.bmp)");
		return FALSE;
	}
	DIBSECTION dibs; //get the description of it
	if(!GetObject(hBmpTiles, sizeof(dibs), &dibs))
	{
		DeleteObject(hBmpTiles);
		Error("GetObject", "unable to get the information from preview.bmp");
		return FALSE;
	}
	if(dibs.dsBmih.biBitCount != 4) //make sure it's 4 bits
	{
		DeleteObject(hBmpTiles);
		Error("CLevel::GeneratePreview", "the preview tile bitmap preview.bmp is the wrong format");
		return FALSE;
	}
	
	HDC hDCTiles = CreateCompatibleDC(NULL); //select the preview tiles bitmap into a DC
	SelectObject(hDCTiles, hBmpTiles);
	RGBQUAD argb[16];
	if(!GetDIBColorTable(hDCTiles, 0, 16, argb)) //get its color table
	{
		DeleteDC(hDCTiles);
		DeleteObject(hBmpTiles);
		Error("GetDIBColorTable", "unable to read the palette from preview.bmp");
		return FALSE;
	}

	//320x255 is the magic bitmap size... so set up the bitmap header for this
	BITMAPINFOHEADER bih = {sizeof(BITMAPINFOHEADER), 320, 225, 1, 4, BI_RGB, 0, 0, 0, 0, 0};
	struct {BITMAPINFOHEADER h; RGBQUAD pal[16];} bi; //it's LIKE a BITMAPINFO structure, but better
	CopyMemory(&bi.h, &bih, sizeof(bih));
	CopyMemory(bi.pal, argb, sizeof(argb)); //now, bi is just like a bitmapinfo struct

	PBYTE pyBits = NULL; //create a new bitmap
	HBITMAP hBmpPreview = CreateDIBSection(NULL, (PBITMAPINFO)&bi, DIB_RGB_COLORS, (PVOID *)&pyBits, NULL, 0);
	if(!hBmpPreview)
	{
		DeleteDC(hDCTiles);
		DeleteObject(hBmpTiles);
		Error("CreateDIBSection", "I messed up somehow generating a preview bitmap");
		return FALSE;
	}

	HDC hDCPreview = CreateCompatibleDC(NULL); //select our new bitmap into a DC
	SelectObject(hDCPreview, hBmpPreview);
	for(INT i = 0; i < LEVEL_HEIGHT; i++) //for each tile high
	{
		for(INT j = 0; j < LEVEL_WIDTH; j++) //for each level tile, blit it in place onto the new preview bitmap (note that the tile positions from the source gfx\preview.bmp correspond to the level tile value)
			BitBlt(hDCPreview, j * 5, i * 5, 5, 5, hDCTiles, m_ayTile[MAKEOFFSET(j, i)] * 5, 0, SRCCOPY);
	}
	for(i = 0; i < 4; i++) //and finally, draw the player start positions
		BitBlt(hDCPreview, m_ayPlayerStart[i * 2 + 0] * 5, m_ayPlayerStart[i * 2 + 1] * 5, 5, 5, hDCTiles, i * 5 + 55, 0, SRCCOPY);

	if(!GdiFlush())//does it matter?
	{
		DeleteDC(hDCTiles);
		DeleteObject(hBmpTiles);
		Error("GdiFlush", "something earlier this function failed");
		return FALSE;
	}
	DeleteDC(hDCTiles);
	DeleteObject(hBmpTiles); //we're done with the preview tiles bitmap, so get it outta here

	BITMAPFILEHEADER bfh = //set up a bitmap file header, because we're gonna dump this puppy to a file
	{
		*(PWORD)"BM",
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 16 + 36000,
		0, 0,
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 16
	};

	//create the new file
	HANDLE hFile = CreateFile(".\\preview.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile)
	{
		DeleteDC(hDCPreview);
		DeleteObject(hBmpPreview);
		Error("CreateFile", "unable to create preview.dat");
		return FALSE;
	}
	
	//write the file header
	DWORD dwSize = sizeof(bfh);
	if(!WriteFile(hFile, &bfh, sizeof(bfh), &dwSize, NULL) || dwSize != sizeof(bfh))
	{
		CloseHandle(hFile);
		DeleteDC(hDCPreview);
		DeleteObject(hBmpPreview);
		Error("WriteFile", "unable to write the preview file header");
		return FALSE;
	}

	//then the info header
	dwSize = sizeof(bih);
	if(!WriteFile(hFile, &bih, sizeof(bih), &dwSize, NULL) || dwSize != sizeof(bih))
	{
		CloseHandle(hFile);
		DeleteDC(hDCPreview);
		DeleteObject(hBmpPreview);
		Error("WriteFile", "unable to write the preview info header");
		return FALSE;
	}

	//and palette (in retrospect, this could have been avoided by writing "bi" instead of "bih" above... but whatever)
	dwSize = sizeof(argb);
	if(!WriteFile(hFile, argb, sizeof(argb), &dwSize, NULL) || dwSize != sizeof(argb))
	{
		CloseHandle(hFile);
		DeleteDC(hDCPreview);
		DeleteObject(hBmpPreview);
		Error("WriteFile", "unable to write the preview palette");
		return FALSE;
	}

	//write the bitmap data
	dwSize = 36000;
	if(!WriteFile(hFile, pyBits, 36000, &dwSize, NULL) || dwSize != 36000)
	{
		CloseHandle(hFile);
		DeleteDC(hDCPreview);
		DeleteObject(hBmpPreview);
		Error("WriteFile", "unable to write the preview bitmap data");
		return FALSE;
	}

	CloseHandle(hFile);
	DeleteDC(hDCPreview);
	DeleteObject(hBmpPreview); //close everything

	return TRUE;
}

//sets a tile in the level to some other value (used by the editor)
VOID CLevel::SetTile(UINT unWhich, BYTE yTile)
{
	if(unWhich < LEVEL_SIZE && yTile <= TILE_DAM_DOWN) //if it's valid
	{
		if(yTile != m_ayTile[unWhich]) //draw it if necessary
			m_ayDraw[unWhich] = TRUE;
		m_ayTile[unWhich] = yTile; //set it
		
		for(INT i = 0; i < 4; i++) //check to make sure the players aren't fucked up now
		{
			if((yTile == TILE_WALL_NORMAL || yTile == TILE_WALL_INDEST || yTile == TILE_WARP)
			&& (INT)unWhich == MAKEOFFSET(m_ayPlayerStart[i * 2 + 0], m_ayPlayerStart[i * 2 + 1]))
			{
				m_ayPlayerStart[i * 2 + 0] = m_ayPlayerStart[i * 2 + 1] = 255; //if so, un-fuck them up
			}
		}
	}
}

//sets a player start to a certain value (used by the editor)
VOID CLevel::SetPlayerStart(UINT unPlayer, BYTE yX, BYTE yY)
{
	if(unPlayer < 4) //only if we're in range
	{
		m_ayPlayerStart[unPlayer * 2 + 0] = yX; //set it (even if it's off screen)
		m_ayPlayerStart[unPlayer * 2 + 1] = yY;

		if(yX < LEVEL_WIDTH && yY < LEVEL_HEIGHT) //if it's on screen
		{
			if(m_ayTile[MAKEOFFSET(yX, yY)] == TILE_WALL_NORMAL || m_ayTile[MAKEOFFSET(yX, yY)] == TILE_WALL_INDEST || m_ayTile[MAKEOFFSET(yX, yY)] == TILE_WARP)
			{
				m_ayTile[MAKEOFFSET(yX, yY)] = TILE_BLANK; //make sure the tile isn't messed up
				m_ayDraw[MAKEOFFSET(yX, yY)] = TRUE;
			}

			for(INT i = 0; i < 4; i++) //and make sure we're not overwriting other players
			{
				if(i == (INT)unPlayer)
					continue;

				if(yX == m_ayPlayerStart[i * 2 + 0] && yY == m_ayPlayerStart[i * 2 + 1])
					m_ayPlayerStart[i * 2 + 0] = m_ayPlayerStart[i * 2 + 1] = 255; //if we are, make it not so
			}
		}
	}
}

//returns the tile value at a specific offset
BYTE CLevel::GetTile(UINT unOffset)
{
	if(unOffset < LEVEL_SIZE)
		return m_ayTile[unOffset];
	else
		return 0;
}

//returns a player's starting position
VOID CLevel::GetPlayerStart(UINT unPlayer, PBYTE pyX, PBYTE pyY)
{
	if(unPlayer < 4)
	{
		*pyX = m_ayPlayerStart[unPlayer * 2 + 0];
		*pyY = m_ayPlayerStart[unPlayer * 2 + 1];
	}
}

//returns TRUE if the level's valid, FALSE if it's invalid
BOOL CLevel::IsValid()
{
	for(INT i = 0; i < 4; i++) //check each player
	{
		if(m_ayPlayerStart[i * 2 + 0] >= LEVEL_WIDTH || m_ayPlayerStart[i * 2 + 1] >= LEVEL_HEIGHT)
			return FALSE; //if the player start is off screen, it's not valid

		BYTE yTile = m_ayTile[MAKEOFFSET(m_ayPlayerStart[i * 2 + 0], m_ayPlayerStart[i * 2 + 1])];
		if(yTile == TILE_WALL_NORMAL || yTile == TILE_WALL_INDEST || yTile == TILE_WARP)
			return FALSE; //if a player's starting on a wall or warp, it's invalid

		for(INT j = 0; j < 4; j++) //if two players are on top of each other
		{
			if(i == j)
				continue;

			if(m_ayPlayerStart[i * 2 + 0] == m_ayPlayerStart[j * 2 + 0] && m_ayPlayerStart[i * 2 + 1] == m_ayPlayerStart[j * 2 + 1])
				return FALSE; //it's not valid
		}
	}

	return TRUE;
}

//resets any images used by the level (only in editor or game)
//returns whethet it succeeds
BOOL CLevel::Reset()
{
	if(m_piScreen && !m_piScreen->Reset())
		return FALSE;

	DrawAll(); //if it was reset, the image contains blackness, so draw everything

	return TRUE;
}

//called on game start
BOOL CLevel::StartGame()
{
	m_dwNextSpawnTime = g_dwCurrentTime + m_loLevOpts.wSpawnFreq * 1000; //set up the timer
	return TRUE;
}

//nothing to do
VOID CLevel::PauseGame()
{
}

//on unpause
VOID CLevel::UnpauseGame()
{
	m_dwNextSpawnTime += g_dwCurrentTime - g_dwPausedTime; //add to timer
}

//clears the tile at unOffset... if it's being cleared because of an explosion, bFromExplosion
//should be true... and if it's from an explosion, bPersistentPowerups should be true if you
//want powerups to stick around, false if they should go away, too
//returns whether it succeeds
BOOL CLevel::Destroy(UINT unOffset, BOOL bFromExplosion, BOOL bPersistentPowerups)
{
	//if it's in range and the tile's not something that can't be destroyed
	if(unOffset < LEVEL_SIZE && m_ayTile[unOffset] != TILE_BLANK && m_ayTile[unOffset] != TILE_WALL_INDEST && m_ayTile[unOffset] != TILE_WARP)
	{
		//if it's not from an explosion and someone destroyed a wall, play the crumble sound
		if(!bFromExplosion && m_ayTile[unOffset] == TILE_WALL_NORMAL && !m_psCrumble->PanPlay(XFROMOFFSET(unOffset), FALSE))
			return FALSE;

		//if it's not from an explosion or it is, but powerups are to be destroyed, or it is exploding, but we're on a normal wall
		if(!bFromExplosion || !bPersistentPowerups || (bFromExplosion && m_ayTile[unOffset] == TILE_WALL_NORMAL))
		{
			if(m_ayTile[unOffset]) //destroy it
			{
				m_ayTile[unOffset] = TILE_BLANK;
				m_ayDraw[unOffset] = TRUE; //draw it
			}
		}
	}

	return TRUE;
}

//actually, this gets a warp destination... silly me for badly naming my functions
//the three offsets that you pass are the offsets of tiles that it CAN'T warp to
//i.e., the offsets of other players' positions
//returns the offset you're supposed to go to
UINT CLevel::GetSpawnDestination(INT nOffset1, INT nOffset2, INT nOffset3)
{
	for(INT i = 0; i < 3; i++)//try to find a blank spot three times by looking
	{
		UINT unOffset = rand() % LEVEL_SIZE; //pick a random tile
		if(m_ayTile[unOffset] != TILE_WALL_NORMAL && m_ayTile[unOffset] != TILE_WALL_INDEST && m_ayTile[unOffset] != TILE_WARP && (INT)unOffset != nOffset1 && (INT)unOffset != nOffset2 && (INT)unOffset != nOffset3)
			return unOffset; //if we find a good one, use it
	}
	
	//if we didn't find it there, we need to compile a list of empty tiles (to insure randomness)
	INT nEmptyTiles = 0;
	UINT aunEmpty[LEVEL_SIZE];
	for(i = 0; i < LEVEL_SIZE; i++)
	{
		if(m_ayTile[i] != TILE_WALL_NORMAL && m_ayTile[i] != TILE_WALL_INDEST && m_ayTile[i] != TILE_WARP && i != nOffset1 && i != nOffset2 && i != nOffset3)
				aunEmpty[nEmptyTiles++] = i; //store each empty tile
	}

	if(!nEmptyTiles)//if this is FUCKED UP, just return a random one... nobody'll notice
	{
		Error("CLevel::GetSpawnDestination", "this level is somehow invalid");
		return (UINT)(rand() % LEVEL_SIZE);
	}

	return aunEmpty[rand() % nEmptyTiles];//pick a random empty tile
}

//called each frame
//returns whether it succeeds
BOOL CLevel::Update()
{
	if(m_ltType != LT_PLAY) //only for those in the game engine
		return TRUE;

	if(m_loLevOpts.wSpawnFreq && g_dwCurrentTime >= m_dwNextSpawnTime) //if it's time to check whether to spawn an item
	{
		if((rand() % 100) < m_loLevOpts.ySpawnChance) //if the check passes
		{
			INT nChoose = rand() % (m_loLevOpts.ySpawnBomns + m_loLevOpts.ySpawnHealth + m_loLevOpts.ySpawnInvul + m_loLevOpts.ySpawnRadUp + m_loLevOpts.ySpawnRadDown + m_loLevOpts.ySpawnDamUp + m_loLevOpts.ySpawnDamDown);
			if(nChoose < m_loLevOpts.ySpawnBomns) //spawn one of the following tiles, depending on how frequent it's supposed to be
			{
				if(!Spawn(TILE_BOMN))
					return FALSE;
			}
			else if(nChoose < m_loLevOpts.ySpawnBomns + m_loLevOpts.ySpawnHealth)
			{
				if(!Spawn(TILE_HEALTH))
					return FALSE;
			}
			else if(nChoose < m_loLevOpts.ySpawnBomns + m_loLevOpts.ySpawnHealth + m_loLevOpts.ySpawnInvul)
			{
				if(!Spawn(TILE_INVUL))
					return FALSE;
			}
			else if(nChoose < m_loLevOpts.ySpawnBomns + m_loLevOpts.ySpawnHealth + m_loLevOpts.ySpawnInvul + m_loLevOpts.ySpawnRadUp)
			{
				if(!Spawn(TILE_RAD_UP))
					return FALSE;
			}
			else if(nChoose < m_loLevOpts.ySpawnBomns + m_loLevOpts.ySpawnHealth + m_loLevOpts.ySpawnInvul + m_loLevOpts.ySpawnRadUp + m_loLevOpts.ySpawnRadDown)
			{
				if(!Spawn(TILE_RAD_DOWN))
					return FALSE;
			}
			else if(nChoose < m_loLevOpts.ySpawnBomns + m_loLevOpts.ySpawnHealth + m_loLevOpts.ySpawnInvul + m_loLevOpts.ySpawnRadUp + m_loLevOpts.ySpawnRadDown + m_loLevOpts.ySpawnDamUp)
			{
				if(!Spawn(TILE_DAM_UP))
					return FALSE;
			}
			else if(nChoose < m_loLevOpts.ySpawnBomns + m_loLevOpts.ySpawnHealth + m_loLevOpts.ySpawnInvul + m_loLevOpts.ySpawnRadUp + m_loLevOpts.ySpawnRadDown + m_loLevOpts.ySpawnDamUp + m_loLevOpts.ySpawnDamDown)
			{
				if(!Spawn(TILE_DAM_DOWN))
					return FALSE;
			}
		}
		m_dwNextSpawnTime += m_loLevOpts.wSpawnFreq * 1000; //wait again
	}

	return TRUE;
}

//draws the level onto piDest
//returns whether it succeeds
BOOL CLevel::Render(CImage * piDest)
{
	if(m_pCopyGraphics && m_piScreen && m_piCopySprites) //if everything's valid
	{
		for(INT i = 0; i < LEVEL_SIZE; i++) //for each tile
		{
			if(m_ayDraw[i]) //if we're supposed to draw it
			{
				RECT rcDest;
				rcDest.left   = XFROMOFFSET(i) << 4; //set up dest rect
				rcDest.top    = YFROMOFFSET(i) << 4;
				rcDest.right  = rcDest.left + 16;
				rcDest.bottom = rcDest.top + 16;
				RECT rcSource;
				switch(m_ayTile[i])
				{
					case TILE_WALL_NORMAL: case TILE_WALL_INDEST: case TILE_INVUL: rcSource.left =  80; break;
					case TILE_RAD_UP: case TILE_RAD_DOWN: case TILE_BOMN:          rcSource.left =  96; break;
					case TILE_DAM_UP: case TILE_DAM_DOWN:                          rcSource.left = 112; break;
					default:                                                       rcSource.left =   0; break;
				}
				switch(m_ayTile[i])
				{
					case TILE_HEALTH: case TILE_WALL_INDEST: case TILE_RAD_DOWN: case TILE_DAM_DOWN: rcSource.top = 16; break;
					case TILE_WARP: case TILE_INVUL: case TILE_BOMN:                                 rcSource.top = 32; break;
					default:                                                                         rcSource.top =  0; break;
				}
				rcSource.right  = rcSource.left + 16; //and source rect
				rcSource.bottom = rcSource.top + 16;

				if(!m_pCopyGraphics->Blit(m_piScreen, &rcDest, 1, m_piCopySprites, &rcSource)) //blit it to our private screen
					return FALSE;

				m_ayDraw[i] = FALSE; //don't draw it again
			}
		}

		RECT rcDest = {0, 0, SCR_WIDTH, SCR_HEIGHT - 48};
		if(!m_pCopyGraphics->Blit(piDest, &rcDest, 1, m_piScreen, NULL)) //then, blit our private screen to the dest
			return FALSE;
	}

	return TRUE;
}

//sets the draw flag for each tile
VOID CLevel::DrawAll()
{
	for(INT i = 0; i < LEVEL_SIZE; i++)
		m_ayDraw[i] = TRUE;
}

//returns the offset of a random, empty tile
//bForPlayer is true if we're getting a random player start point
//or false if we're getting an item spawn location
UINT CLevel::GetRandomEmptyTile(BOOL bForPlayer)
{
	for(INT i = 0; i < 3; i++)//try to find a blank spot three times by looking
	{
		UINT unOffset = rand() % LEVEL_SIZE;
		if(bForPlayer) //if it's for a player
		{
			if(m_ayTile[unOffset] != TILE_WALL_NORMAL && m_ayTile[unOffset] != TILE_WALL_INDEST && m_ayTile[unOffset] != TILE_WARP
			&& (INT)unOffset != MAKEOFFSET(m_ayPlayerStart[0], m_ayPlayerStart[1]) && (INT)unOffset != MAKEOFFSET(m_ayPlayerStart[2], m_ayPlayerStart[3])
			&& (INT)unOffset != MAKEOFFSET(m_ayPlayerStart[4], m_ayPlayerStart[5]) && (INT)unOffset != MAKEOFFSET(m_ayPlayerStart[6], m_ayPlayerStart[7]))
				return unOffset; //if it's not a wall or warp, or any other player's start point, return it
		}
		else //it's for a tile
		{
			if(m_ayTile[unOffset] == TILE_BLANK) //only get blank tiles here
				return unOffset;
		}
	}
	
	//if we didn't find it there, we need to compile a list of empty tiles (to insure randomness)
	INT nEmptyTiles = 0;
	UINT aunEmpty[LEVEL_SIZE];
	for(i = 0; i < LEVEL_SIZE; i++)
	{
		if(bForPlayer)
		{
			if(m_ayTile[i] != TILE_WALL_NORMAL && m_ayTile[i] != TILE_WALL_INDEST && m_ayTile[i] != TILE_WARP
			&& i != MAKEOFFSET(m_ayPlayerStart[0], m_ayPlayerStart[1]) && i != MAKEOFFSET(m_ayPlayerStart[2], m_ayPlayerStart[3])
			&& i != MAKEOFFSET(m_ayPlayerStart[4], m_ayPlayerStart[5]) && i != MAKEOFFSET(m_ayPlayerStart[6], m_ayPlayerStart[7]))
				aunEmpty[nEmptyTiles++] = i; //add tiles that aren't walls or warps or other players' start points
		}
		else
		{
			if(m_ayTile[i] == TILE_BLANK) //add only blank tiles
				aunEmpty[nEmptyTiles++] = i;
		}
	}

	if(!nEmptyTiles)//if this is FUCKED UP, just return a random one... nobody'll notice
	{
		Error("CLevel::GetRandomEmptyTile", "this level is somehow invalid");
		return (UINT)(rand() % LEVEL_SIZE);
	}

	return aunEmpty[rand() % nEmptyTiles];//pick a random empty tile
}

//spawns yTile into the level at a random place	(only used by LT_PLAY)
//returns whether it succeeds
BOOL CLevel::Spawn(BYTE yTile)
{
	if(yTile > TILE_DAM_DOWN) //if it's not a valid tile
		return TRUE;

	for(INT i = 0; i < 3; i++)//try to find a blank spot three times by looking
	{
		UINT unOffset = rand() % LEVEL_SIZE;
		if(m_ayTile[unOffset] == TILE_BLANK)
		{
			m_ayTile[unOffset] = yTile; //add it
			m_ayDraw[unOffset] = TRUE; //draw it
			if(!m_psSpawn->PanPlay(XFROMOFFSET(unOffset), FALSE)) //play the sound
				return FALSE;
			return TRUE;
		}
	}
	
	//if we didn't find it there, we need to compile a list of empty tiles (to insure randomness)
	INT nEmptyTiles = 0;
	UINT aunEmpty[LEVEL_SIZE];
	for(i = 0; i < LEVEL_SIZE; i++)
	{
		if(m_ayTile[i] == TILE_BLANK)
			aunEmpty[nEmptyTiles++] = i;
	}

	if(nEmptyTiles) //if there's an empty spot
	{
		UINT unOffset = aunEmpty[rand() % nEmptyTiles];
		m_ayTile[unOffset] = yTile;
		m_ayDraw[unOffset] = TRUE;
		if(!m_psSpawn->PanPlay(XFROMOFFSET(unOffset), FALSE))
			return FALSE;
		return TRUE;
	}

	return TRUE; //if it gets to here, nothing was spawned, but that means the level is full, so do nothing
}