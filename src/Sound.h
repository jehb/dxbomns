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
// Sound.h -- the header for the game's sound system.  Some day this         //
// might become a library all its own.  Basically, it's just a DX wrapper.   //
// Anyway, look at the .cpp file for more info.                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __SOUND_H__
#define __SOUND_H__


class CSoundSystem;
class CSound;


#define DIRECTSOUND_VERSION 0x0800
#include <dsound.h>


typedef struct _SOUND_VARS //internal sound variables
{
	IDirectSound8 * pDS; //all we need here is the dsound object
} SOUND_VARS, * PSOUND_VARS;


class CSoundSystem //our sound system class
{
	public:
		CSoundSystem();
		~CSoundSystem();

		BOOL Initialize(HWND); //initialize it

		CSound * CreateSound(PCSTR); //create a sound effect

	private:
		SOUND_VARS m_svInnards; //internal vars

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CSoundSystem(const CSoundSystem &);
		VOID operator = (const CSoundSystem &);
};

class CSound //a sound file object
{
	public:
		CSound();
		~CSound();

		BOOL Initialize(IDirectSound8 *, PCSTR); //initialize it

		BOOL PanPlay(INT, BOOL); //pan it, then play
		BOOL Play(BOOL);         //play w/o pan
		VOID Stop();             //stop playing

	private:
		PSTR                  m_szFile;    //the file associated with the effect
		IDirectSoundBuffer8 * m_pdsbSound; //the actual buffer

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CSound(const CSound &);
		VOID operator = (const CSound &);
};


CSoundSystem * CreateSoundSystem(HWND); //create our sound system


#endif