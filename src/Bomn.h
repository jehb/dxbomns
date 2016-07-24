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
// Bomn.h -- definitions and such for the bomn (and explosion) system in the //
// game engine.  See Bomn.cpp for more info.                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __BOMN_H__
#define __BOMN_H__


class CBomnSystem; //defined below
struct BOMNNODE;
struct EXPLOSIONNODE;


#include "Graphics.h"
#include "Sound.h"
#include "World.h"


class CBomnSystem //the class that handles all the bomns and explosions
{
	public:
		CBomnSystem(); //see the .cpp file for details on these member functions
		~CBomnSystem();

		BOOL Initialize(CWorld *, CGraphics *, CImage *, CSoundSystem *);	//initializes the class

		BOOL StartGame();   //on game start
		VOID PauseGame();   //on game pause
		VOID UnpauseGame(); //on game unpause

		BOOL AddBomn(INT, INT, INT, INT, INT, INT); //someone dropped a bomn
		BOOL IsBomnAt(INT, INT);                    //check for whether a bomn is at position x,y

		BOOL Reset();                               //reset the images contained here

		BOOL Update();          //update the system
		BOOL Render(CImage *);  //render the system

	private:
		BOOL Explode(const BOMNNODE *);       //turn a bomn into an explosion

		CWorld        * m_pCopyWorld;         //a copy of the world we're in

		CGraphics     * m_pCopyGraphics;      //copy of the graphics system we're using
		CImage        * m_piCopySprites;      //and the sprites image
		CSoundSystem  * m_pCopySoundSystem;   //and the sound system

		CImage        * m_piExplosionSprites; //will be created, this is the image from where ExplosionScreen is blitted
		CImage        * m_piExplosionScreen;  //blitted over the whole screen each frame, holds current explosions

		CSound        * m_psTick;             //the tick sound (already allocated in game.cpp, but I was too lazy to put a copy here)

		BOMNNODE      * m_pBomnHead;          //linked list of current bomns
		EXPLOSIONNODE * m_pExplosionHead;     //list of explosions

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CBomnSystem(const CBomnSystem &);
		VOID operator = (const CBomnSystem &);
};


extern DWORD g_dwCurrentTime; //needed for the structs below

struct BOMNNODE //linked list of bomns
{
	BOMNNODE(INT x, INT y, INT o, INT r, INT d, INT t, BOMNNODE * n)
	{
		nX = x;
		nY = y;
		nOwner = o;      //set up the deals
		nRadius = r;
		nDamage = d;
		dwExplodeTime = g_dwCurrentTime + t * 1000;
		bAboutCalled = FALSE;
		bDraw = TRUE;
		bShowTime = TRUE;
		dwNextBlinkTime = g_dwCurrentTime + 250;

		pNext = n;
		if(pNext)
			pNext->pPrev = this;
		pPrev = NULL;
	}

	~BOMNNODE() SAFEDELETE(pNext) //this enables us to just delete the head and know the entire list was dealloc'd

	INT        nX, nY;
	INT        nOwner;          //player that owns it
	INT        nRadius;         //how big it should explode to
	INT        nDamage;         //how much damage it should do when exploding
	DWORD      dwExplodeTime;   //when it explodes
	BOOL       bAboutCalled;    //whether AboutToExplode in the world was called
	BOOL       bDraw;           //whether to draw it
	BOOL       bShowTime;       //show time remaining vs. show generic active bomn sprite
	DWORD      dwNextBlinkTime; //when it's set to blink

	BOMNNODE * pNext;
	BOMNNODE * pPrev;
};

struct EXPLOSIONNODE //linked list of explosions
{
	EXPLOSIONNODE(INT x, INT y, INT r, EXPLOSIONNODE * n)
	{
		nX = x;
		nY = y;
		nRadius = r;
		bDraw = TRUE;  //set up the deals
		nStage = 0;
		dwNextSwitchTime = g_dwCurrentTime + 100;
		bKill = FALSE;

		pNext = n;
		if(pNext)
			pNext->pPrev = this;
		pPrev = NULL;
	}

	~EXPLOSIONNODE() SAFEDELETE(pNext) //so we can just delete head and know whole list was dealloc'd

	INT             nX, nY;
	INT             nRadius;

	BOOL            bDraw;            //whether it needs to be drawn
	INT             nStage;           //what stage it's in (0-10)
	DWORD           dwNextSwitchTime; //when it switches stages
	BOOL            bKill;            //flag to delete this element next update

	EXPLOSIONNODE * pNext;
	EXPLOSIONNODE * pPrev;
};


CBomnSystem * CreateBomnSystem(CWorld *, CGraphics *, CImage *, CSoundSystem *); //creates a bomn system class


#endif