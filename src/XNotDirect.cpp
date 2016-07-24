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
// XNotDirect.cpp -- code that simplifies going into fullscreen DX mode...   //
// anyone can call it and then have a valid graphics, soundsystem, and       //
// input object... it's used by Game.cpp and Editor.cpp to do just that.     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include "DXBomns.h"
#include "Graphics.h"
#include "Sound.h"
#include "Input.h"
#include "XNotDirect.h"

#pragma comment(lib, "dxguid")


extern HINSTANCE    g_hInst;      //from DXBomns.cpp
extern HWND         g_hCurrentWnd;

CGraphics         * g_pGraphics      = NULL; //the graphics object (use extern to use it in other modules)

CSoundSystem      * g_pSoundSystem   = NULL; //soundsystem (use extern again)

CInput            * g_pInput         = NULL; //and the input object, with keyboard and mouse ready to go
CDevice           * g_pdKeyboard     = NULL; //(extern these to use them in other modules)
CDevice           * g_pdMouse        = NULL;


//goes to fullscreen DX mode, sets up the soundsystem and input, and creates the keyboard and mouse
//returns whether it succeeded
//pfnOnKey and pfnOnMouse are used by g_pdKeyboard and g_pdMouse, so refer to the CDevice section in Input.cpp
BOOL InitDX(BOOL (*pfnOnKey)(PINPUT_EVENT), BOOL (*pfnOnMouse)(PINPUT_EVENT))
{
	ShowCursor(FALSE); //get that outta here

	g_pGraphics = CreateGraphics(g_hCurrentWnd, SCR_WIDTH, SCR_HEIGHT, SCR_BPP); //go to full-screen shits
	if(!g_pGraphics)
	{
		CloseDX();
		return FALSE;
	}

	g_pSoundSystem = CreateSoundSystem(g_hCurrentWnd); //take control of audio
	if(!g_pSoundSystem)
	{
		CloseDX();
		return FALSE;
	}

	g_pInput = CreateInput(g_hInst, g_hCurrentWnd); //grab input
	if(!g_pInput)
	{
		CloseDX();
		return FALSE;
	}
	g_pdKeyboard = g_pInput->CreateDevice(DT_KEYBOARD, pfnOnKey); //create keyboard
	if(!g_pdKeyboard)
	{
		CloseDX();
		return FALSE;
	}
	g_pdMouse = g_pInput->CreateDevice(DT_MOUSE, pfnOnMouse); //create mouse
	if(!g_pdMouse)
	{
		CloseDX();
		return FALSE;
	}

	return TRUE;
}

//dealloc DX stuff, go to normal mode
VOID CloseDX()
{
	ShowCursor(TRUE);

	SAFEDELETE(g_pdMouse)
	SAFEDELETE(g_pdKeyboard)
	SAFEDELETE(g_pInput)

	SAFEDELETE(g_pSoundSystem)
	
	SAFEDELETE(g_pGraphics)
}