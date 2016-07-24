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
// DXWindow.cpp -- code for the window that's used by Game.cpp and           //
// Editor.cpp, a window for generic full-screen DX use                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include "resource.h"
#include "DXBomns.h"
#include "DXWindow.h"
#include "EditorDialog.h"


extern HINSTANCE g_hInst;       //from DXBomns.cpp
extern HWND      g_hCurrentWnd;

BOOL             g_bRegistered        = FALSE; //have we registered the class yet?

VOID          (* g_pfnExit)()         = NULL; //function to call if someone tries to close the window
VOID          (* g_pfnActivate)(BOOL) = NULL; //and function to call when the app goes active or inactive


//creates and enters the DX window (sets g_hCurrentWnd appropriately)
//destroy g_hCurrentWnd before calling this!
//pfnEx is a pointer to a function that accepts no parameters and returns nothing...
//it's called when it processes the WM_CLOSE message
//and pfnAc is a pointer to a func that accepts a BOOL = TRUE if app becoming active, FALSE if app going inactive
//and returns nothing... it's called in response to WM_ACTIVEATEAPP message
//anyway... THIS function returns whether it succeeded
BOOL CreateDXWindow(VOID (* pfnEx)(), VOID (* pfnAc)(BOOL))
{
	g_pfnExit = pfnEx;     //set the func pointers
	g_pfnActivate = pfnAc;

	if(!g_bRegistered) //if we haven't registered the class yet
	{
		g_bRegistered = TRUE;
		WNDCLASSEX wcex = {sizeof(WNDCLASSEX), 0, (WNDPROC)DXWindowProc, 0, 0, g_hInst, LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON)), LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_APPWORKSPACE + 1), NULL, "DirectX Window", (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_SHARED)};
		if(!RegisterClassEx(&wcex)) //register it
		{
			Error("RegisterClassEx", "unable to register the DirectX Window class");
			return FALSE;
		}
	}

	g_hCurrentWnd = CreateWindowEx(0, "DirectX Window", "DirectX Bomns", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInst, NULL);
	if(!g_hCurrentWnd) //create the window
	{
		Error("CreateWindowEx", "unable to create the DX window");
		return FALSE;
	}

	ShowWindow(g_hCurrentWnd, SW_NORMAL); //show 'n' update
	UpdateWindow(g_hCurrentWnd);

	return TRUE;
}

//the window proc for the DX window
LRESULT CALLBACK DXWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_PAINT: //don't worry about painting it; it's fine
			ValidateRect(hWnd, NULL);
			return 0;

		case WM_ACTIVATEAPP: //call the activate function
			if(g_pfnActivate)
				g_pfnActivate((BOOL)wParam);
			break;

		case WM_CLOSE: //call the exit function
			if(g_pfnExit)
				g_pfnExit();
			break;

		case WM_DESTROY: //clean up
			g_pfnActivate = NULL;
			g_pfnExit = NULL;
			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}