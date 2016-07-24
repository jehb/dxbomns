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
// World.h -- the header for the game's world class.  The world controls     //
// interactions between level and player and bomns.  Look at the .cpp for    //
// more info.                                                                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __WORLD_H__
#define __WORLD_H__


class CWorld;


#include "Graphics.h"
#include "Sound.h"
#include "Font.h"
#include "Level.h"
#include "Player.h"
#include "Bomn.h"


#define SND_STEP        0 //sound array offsets -- the player move sound
#define SND_NOGO        1 //can't move
#define SND_WARP        2 //warp
#define SND_PUNCH       3 //hit another player
#define SND_HEALTH      4 //collect health
#define SND_INVUL       5 //collect invul
#define SND_VUL         6 //lose invul
#define SND_COLLECT     7 //collect other powerup
#define SND_NEEDHEALTH  8 //player needs health badly
#define SND_DEATH       9 //player dies
#define SND_DROP       10 //player drops bomn
#define SND_NODROP     11 //can't drop
#define SND_EXPLODE    12 //bomn explodes
#define SND_UHOH       13 //bomn about to explode and player is in radius
#define NUM_SOUNDS     14 //how many


const CHAR * const g_cacszSoundFile[NUM_SOUNDS] = //the sound file names
{
	".\\sfx\\step.wav",
	".\\sfx\\nogo.wav",
	".\\sfx\\warp.wav",
	".\\sfx\\punch.wav",
	".\\sfx\\health.wav",
	".\\sfx\\invul.wav",
	".\\sfx\\vul.wav",
	".\\sfx\\collect.wav",
	".\\sfx\\needhealth.wav",
	".\\sfx\\death.wav",
	".\\sfx\\drop.wav",
	".\\sfx\\nodrop.wav",
	".\\sfx\\explode.wav",
	".\\sfx\\uhoh.wav",
};


class CWorld //and our world class at last
{
	public:
		CWorld();
		~CWorld();

		BOOL Initialize(CGraphics *, CSoundSystem *); //initialize it

		BOOL StartGame();   //on game start
		VOID PauseGame();   //pause
		VOID UnpauseGame(); //unpause

		BOOL KeyHit(BYTE);                                 //someone hit a key
		BOOL TryMove(INT, INT, INT, PINT, PINT);           //a player is trying to move
		BOOL TryDrop(INT, INT, INT, INT, INT, PINT, PINT); //a player is trying to drop
		BOOL PlayerLoseInvul(INT);                         //no more invul for a player
		BOOL BomnAboutToExplode(INT, INT, INT);            //check for uhoh sound
		BOOL Explode(INT, INT, INT, INT, INT);             //explode a bomn

		INT  Winner();                                   //who's the winner, if any
		VOID GetPlayerInfo(INT, PINT, PINT, PINT, PINT); //gnab a player's info

		BOOL Reset(); //reset the images

		BOOL Update();                        //each frame
		BOOL Render(CImage *, BOOL, CFont *); //render

	private:
		GAMOPTS        m_goGameOpts; //copies of the options and config
		KEYCONFIG      m_kcConfig;

		CLevel       * m_pLevel;                //the level
		CPlayer      * m_apPlayer[MAX_PLAYERS]; //all the players
		INT            m_nPlayers;              //how many players (somewhat redundant, as it's also defined in the gameopts)

		CBomnSystem  * m_pBomnSystem;           //the bomn system

		CImage       * m_piSprites;             //the sprites image

		CSound       * m_apsSound[NUM_SOUNDS];  //all the sounds used here

		CGraphics    * m_pCopyGraphics;     //copies of stuff
		CSoundSystem * m_pCopySoundSystem;


		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CWorld(const CWorld &);
		VOID operator = (const CWorld &);
};


CWorld * CreateWorld(CGraphics *, CSoundSystem *); //create a world


#endif