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
// Player.h -- defines the player class et alii.  See the .cpp for more info //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __PLAYER_H__
#define __PLAYER_H__


class CPlayer;


#include "Graphics.h"
#include "Opts.h"
#include "World.h"


#define MAX_PLAYERS 4 //max players in game at once

typedef enum _MOVETYPE //how are we requesting to move?
{
	MT_UP,
	MT_DOWN,
	MT_LEFT,
	MT_RIGHT,

	MT_FORCE_DWORD = 0x7fffffff
} MOVETYPE, * PMOVETYPE;


class CPlayer //the player class
{
	public:
		CPlayer();
		~CPlayer();

		BOOL Initialize(CGraphics *, CImage *, INT, INT, const GAMOPTS *); //initialize it

		BOOL Reset(); //reset the graphics

		BOOL StartGame();   //on game start
		VOID PauseGame();   //game pause
		VOID UnpauseGame(); //game unpause

		VOID GetPosition(PINT, PINT);                //get player position
		BOOL IsInvul() const;                        //is it invul?
		BOOL Move(INT, MOVETYPE, CWorld *);          //the world is telling us we want to move (the world controls the keys, you see)
		BOOL Drop(INT, CWorld *);                    //the world is telling us we want to drop a bomn
		BOOL Hurt(INT, INT, INT, PBOOL, PBOOL);      //the world is telling us we are taking damage
		INT  Health() const;                         //how much health is left
		INT  NumBomns() const;                       //how many bomns we got
		VOID GetInfo(PINT, PINT, PINT, PINT) const;  //get health, bomns, rad, and dam
		BOOL Collect(BYTE);                          //the world is telling us what we're standing on, to collect it if necessary
		VOID BomnExplode();                          //one of our bomns exploded (decrease numbomnsout)

		BOOL Update(CWorld *);       //each frame
		BOOL Render(CImage *, INT);  //render it

	private:
		INT                m_nX, m_nY;

		INT                m_nHealth;   //our health
		INT                m_nBomns;    //how many bomns we have to drop
		INT                m_nBomnsOut; //how many bomns of ours are out
		INT                m_nRad;      //how big our bomns are
		INT                m_nDam;      //and how much damage they do
		BOOL               m_bInvul;    //are we invul
		DWORD              m_dwNoInvulTime; //if so, when do we become not invul
		
		CGraphics        * m_pCopyGraphics; //copies
		CImage           * m_piCopySprites;

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CPlayer(const CPlayer &);
		VOID operator = (const CPlayer &);
};


CPlayer * CreatePlayer(CGraphics *, CImage *, INT, INT, const GAMOPTS *); //create a player object


#endif