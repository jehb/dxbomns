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
// DXBomns.cpp -- the code for WinMain, the startup dialog, and the game's   //
// error logging system.  Nothin' too complex.                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"
#include "DXBomns.h"
#include "EditorDialog.h"


HINSTANCE       g_hInst        = NULL;
HWND            g_hCurrentWnd  = NULL; //gets overwritten every time a new window is created
HANDLE          g_hHeap        = NULL; //used for allocating big chunks various places

BOOL            g_bError       = FALSE; //set by __logerror

BOOL         (* g_pfnUpdate)() = NULL; //if set, WinMain'll call it each frame


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, PSTR, INT) //we don't give a shit about hPrevInst, szCmdLine, or nCmdShow
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, "DirectX Bomns"); //create a mutex so only one instance can be open
	if(GetLastError() == ERROR_ALREADY_EXISTS) //if there's another instance already
	{
		CloseHandle(hMutex);
		return 0;
	}

	g_hHeap = HeapCreate(0, 0, 0); //create the game's dynamic heap
	if(!g_hHeap)
	{
		MessageBox(NULL, "HeapCreate failed.  You're screwed.  Email me (Toxite@aol.com) if you see this.", "DXBomns Error", MB_OK | MB_ICONERROR);
		CloseHandle(hMutex);
		return 0;
	}

	g_hInst = hInst;

	srand(GetTickCount()); //is there a better place for this?

	if(!EnterStartupDialog()) //create startup dialog (it sets g_hCurrentWnd)
	{
		MessageBox(NULL, "CreateDialog failed.  Look at error.txt if you like, but it won't help.  Email me (Toxite@aol.com) if you see this.", "DXBomns Error", MB_OK | MB_ICONERROR);
		HeapDestroy(g_hHeap);
		CloseHandle(hMutex);
		return 0;
	}

	MSG msg;
	while(666)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //windows stuff
		{
			if(msg.message == WM_QUIT)
			{
				break;
			}
			if(!IsDialogMessage(g_hCurrentWnd, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(g_pfnUpdate)
				g_pfnUpdate(); //if someone set the update deal, update it
		}

		if(g_bError)
			break;
	}

	if(IsWindow(g_hCurrentWnd))      //if, in the process of deallocating stuff, someone forgot to destroy the window, do it here
		DestroyWindow(g_hCurrentWnd);

	if(g_bError)
		MessageBox(NULL, "DirectX Bomns encountered an internal error.  Check error.txt to see if you can figure out what's going on.  You probably won't be able to gain any useful information from error.txt, but if you'll email it to me (Toxite@aol.com) along with a description of what you were doing in DXBomns at the time (and a brief description of your computer), I'll more than likely be able to help you.", "DXBomns Error", MB_OK | MB_ICONERROR);

	HeapDestroy(g_hHeap);
	CloseHandle(hMutex);
	return 0;
}

//goes to the startup dialog
//note: destroy g_hCurrentWnd before calling this
//returns whether it succeeded
BOOL EnterStartupDialog()
{
	g_hCurrentWnd = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_STARTUP), NULL, (DLGPROC)StartupDialogProc);
	if(!g_hCurrentWnd)
	{
		Error("CreateDialog", "unable to create the startup dialog");
		return FALSE;
	}

	return TRUE;
}

//window proc for startup dialog
INT_PTR CALLBACK StartupDialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			HICON hIconBig = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
			HICON hIconSmall = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_SHARED);
			SendMessage(hWndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
			SendMessage(hWndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
			HMENU hSysMenu = GetSystemMenu(hWndDlg, FALSE);        //set up icons and app menu
			EnableMenuItem(hSysMenu, SC_SIZE, MF_GRAYED);
			EnableMenuItem(hSysMenu, SC_MAXIMIZE, MF_GRAYED);
			return TRUE;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDCANCEL:
					DestroyWindow(g_hCurrentWnd);
					PostQuitMessage(0);
					return TRUE;

				case ID_CREDITS:
				{
					CHAR szOut[2048];
					sprintf(szOut, "%s compiled on %s at %s.  Copyright © 1998-2002 Toxite for Allied Computers.  DXBomns is open source and freeware, so copy and distribute it as you wish (please read the license in readme.txt before doing so, though)!\n\nThe Bomns website is now http://bomns.sourceforge.net/ (there is also a lot of other cool stuff there, too).\n\nConcept, code, art, and sounds by Charles Lindsay (Toxite).  The following people helped make DXBomns possible: Jasón Baker (Shadow), Keith Fancher, Sam Wilson, Tristan Tager, and Tangi Smallwood.  My thanks to the following beta testers: Jasón Baker (again), Keith Fancher (also again), Sam Wilson (again again) and Golroc (the only stranger who bothered to email me with feedback during the test!).  Also, I'd like to thank Sourceforge (http://sourceforge.net/) for being really cool.\n\nI (Toxite) can be reached at Toxite@aol.com.  If have anything to say about this game (comments, questions, criticism, et cetera) or you find any bugs in it, please email me there.\n\nThanks!", DXBOMNS_VER_STR_SEMILONG, __DATE__, __TIME__);
					MessageBox(hWndDlg, szOut, "About DirectX Bomns", MB_OK | MB_ICONINFORMATION);
					return TRUE;
				}

				case IDOK:
					DestroyWindow(g_hCurrentWnd);
					EnterEditorDialog(); //go to editor dialog
					return TRUE;
			}
			break;
	}
	
	return FALSE;
}

//__logerror sets the g_bError flag, so the app'll exit, and writes a string to error.txt.
//This could be improved a great deal by making it accept variable parameters and using
//vsprintf to set up the string it actually writes, but since I use the Error() macro
//to hide this function so that the __FILE__ and __LINE__ are always included, that's
//kind of hard to do.  I COULD have just used sprintf a lot before calling Error()
//(so that it would actually TELL you what file it couldn't load, as opposed to just
//saying "a file", but I'm too lazy.  It's good enough.
//Anyway, cszFile is the name of the source file that encountered the error (__FILE__),
//unLine is __LINE__ that called __logerror, cszFunc is the function call that failed,
//and cszMsg is a possible reason for failure.
VOID __logerror(PCSTR cszFile, UINT unLine, PCSTR cszFunc, PCSTR cszMsg)
{
	HANDLE hFile = CreateFile("error.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, (g_bError ? OPEN_ALWAYS : CREATE_ALWAYS), FILE_ATTRIBUTE_NORMAL, NULL);

	if(!g_bError)//if it's the first time for an error
	{
		CHAR szIntro[1024];
		sprintf(szIntro, "%s compiled on %s at %s\r\n\r\n", DXBOMNS_VER_STR_SEMILONG, __DATE__, __TIME__);
		DWORD dwSize;
		WriteFile(hFile, szIntro, strlen(szIntro), &dwSize, NULL);

		g_bError = TRUE;
	}
	else
		SetFilePointer(hFile, 0, NULL, FILE_END); //else, append, don't overwrite

	CHAR szOut[1024];
	sprintf(szOut, "Error in source file %s, at line %lu: %s failed -- %s\r\n", cszFile, unLine, cszFunc, cszMsg);
	DWORD dwSize;
	WriteFile(hFile, szOut, strlen(szOut), &dwSize, NULL);

	CloseHandle(hFile);
}