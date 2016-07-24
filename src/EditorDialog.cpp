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
// EditorDialog.cpp -- the source for the editor dialog box, not to be       //
// confused with Editor.cpp, which handles the full-screen map editor.  Most //
// of this file is in the editor window proc.                                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include <stdio.h>

#include <afxres.h>
#include "resource.h"
#include "DXBomns.h"
#include "Level.h"
#include "Game.h"
#include "Editor.h"
#include "Opts.h"
#include "EditorDialog.h"


extern HINSTANCE g_hInst;		  //both from DXBomns.cpp
extern HWND      g_hCurrentWnd;

extern INT       g_anKills[4];  //both from Game.cpp
extern INT       g_anDeaths[4];


//goes to the editor dialog box, and sets g_hCurrentWnd as such
//returns whether it succeeded
//note: destroy g_hCurrentWnd before calling this!
BOOL EnterEditorDialog()
{
	g_hCurrentWnd = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_EDITOR), NULL, (DLGPROC)EditorDialogProc);
	if(!g_hCurrentWnd)
	{
		Error("CreateDialog", "unable to create the editor dialog");
		return FALSE;
	}

	return TRUE;
}

//the window proc for the editor dialog
//one hefty mo fo
INT_PTR CALLBACK EditorDialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CLevel * pLevel = NULL;
	static HBITMAP hBmpPreview = NULL;

	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			HICON hIconBig = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
			HICON hIconSmall = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 16, 16, LR_SHARED);
			SendMessage(hWndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
			SendMessage(hWndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
			HMENU hSysMenu = GetSystemMenu(hWndDlg, FALSE);
			EnableMenuItem(hSysMenu, SC_SIZE, MF_GRAYED);
			EnableMenuItem(hSysMenu, SC_MAXIMIZE, MF_GRAYED);  //set up icons/appmenus

			GrabOpts(); //tells Opts.cpp to read in the .dat and .cfg files

			APPOPTS ao;
			GetAppOpts(&ao); //copy the app opts here, set up the child windows
			SendMessage(GetDlgItem(hWndDlg, IDC_APP_FPS), BM_SETCHECK, (ao.yFPS == 0 ? BST_UNCHECKED : BST_CHECKED), 0);
			SendMessage(GetDlgItem(hWndDlg, IDC_APP_AUDIO), BM_SETCHECK, (ao.yAudio == 0 ? BST_UNCHECKED : BST_CHECKED), 0);

			GAMOPTS go;
			GetGamOpts(&go); //copy game opts here, set up child windows
			SetDlgItemInt(hWndDlg, IDC_GAM_NPLAYERS, go.yPlayers, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_STARTHEALTH, go.yStartHealth, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_STARTBOMNS, go.yStartBomns, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_STARTRADIUS, go.yStartRadius, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_STARTDAMAGE, go.yStartDamage, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_MAXHEALTH, go.yMaxHealth, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_MAXBOMNS, go.yMaxBomns, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_MAXRADIUS, go.yMaxRadius, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_MAXDAMAGE, go.yMaxDamage, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_MINRADIUS, go.yMinRadius, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_MINDAMAGE, go.yMinDamage, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_TIMELIMIT, go.wTimeLimit, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_MAXBOMNSOUT, go.yMaxBomnsOut, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_INVULTIME, go.yInvulTime, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_BOMNTIMER, go.yBomnTimer, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_HEALTHPERPOWERUP, go.yHealthPerPowerup, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GAM_PUNCHDAMAGE, go.yPunchDamage, FALSE);
			SendMessage(GetDlgItem(hWndDlg, IDC_GAM_PERSISTENTPOWERUPS), BM_SETCHECK, (go.yPersistentPowerups == 0 ? BST_UNCHECKED : BST_CHECKED), 0);

			LEVOPTS lo;
			GetLevOpts(&lo); //finally, level opts
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNFREQ, lo.wSpawnFreq, FALSE);
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNCHANCE, lo.ySpawnChance, FALSE);
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNBOMNS, lo.ySpawnBomns, FALSE);
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNHEALTH, lo.ySpawnHealth, FALSE);
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNINVUL, lo.ySpawnInvul, FALSE);
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNRADUPS, lo.ySpawnRadUp, FALSE);
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNRADDOWNS, lo.ySpawnRadDown, FALSE);
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNDAMUPS, lo.ySpawnDamUp, FALSE);
			SetDlgItemInt(hWndDlg, IDC_LEV_SPAWNDAMDOWNS, lo.ySpawnDamDown, FALSE);

			pLevel = CreateLevel(LT_GENERATEANDDUMP, NULL, NULL, NULL); //make a level that can generate stuff and dump to files
			if(pLevel)
			{
				if(GetFileAttributes(".\\lev.dat") == -1) //if lev.dat doesn't exist
					pLevel->DumpToFile(".\\lev.dat");      //dump the blank level to it
				pLevel->LoadFromFile(".\\lev.dat");       //read it in, no matter what
				pLevel->GeneratePreview();                //and generate a preview bitmap

				hBmpPreview = (HBITMAP)LoadImage(NULL, ".\\preview.dat", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE); //set up the preview bitmap, for the big custom draw button
			}
			else
			{
				EnableWindow(GetDlgItem(hWndDlg, ID_EDIT), FALSE);  //um... I guess this code really isn't necessary,
				EnableWindow(GetDlgItem(hWndDlg, ID_LOAD), FALSE);  //but when I wrote it, it seemed that way.  All it
				EnableWindow(GetDlgItem(hWndDlg, ID_SAVE), FALSE);  //does is make things un-clickable if the level fails
				EnableWindow(GetDlgItem(hWndDlg, ID_GEN), FALSE);   //to alloc (does this ever happen?  No.)
				EnableWindow(GetDlgItem(hWndDlg, ID_CLEAR), FALSE);
			}
			return TRUE;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_HELP: //someone pressed help...GOD these are long strings... oh well
					if(MessageBox(hWndDlg, "Most of this dialog should be pretty self-explanatory, but I'll try to offer any wisdom on the subject I can.\n\nStarting with the Game Options section, \"Players\" is of course the number of players in the game (0-4).\nThe table that follows defines the amount of health (1-10), number of bomns (0-99), explosion radius (0-64), and explosion damage (0-10) at the start of the game, as well as defining the maximum and minimum (where applicable) values for those variables (also note that the minimum must be less than or equal to the maximum, and the starting values must be between or equal to the minimum and the maximum).\nIf you wish to put a time limit on the match (0 for no time limit, or 1-65535 seconds), specify it in \"Time Limit in s, 0 = None\".\n\"Bomn Timer Length in s\" allows you to specify how much time should elapse after dropping a bomn before it explodes (0-10 seconds).\n\"Invul. Time in s\" is the amount of time a player remains invulnerable after picking up an invulnerability (0-60 seconds).\nGive the number of bomns each player can drop at a time in \"Max Live Bomns per Player\" (0-10).\nSpecify how much damage is done when any player bumps into another player in \"Punch Damage\" (0-10).\n\"Health per Powerup\" is the amount of health added to a player's life when that player collects a health powerup (0-10).\nCheck \"Persistent Powerups\" to stop explosions from getting rid of powerups in the level.\n\nCheck \"Show FPS\" to display how many frames per second the game is getting.\nIf you wish to disable audio, uncheck \"Audio Enabled\".\n\n\nPress OK to see more help, or Cancel to go back to the editor.", "DXBomns Help", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
						MessageBox(hWndDlg, "Moving on... under Level Options, click the big bitmap thing to edit the level.\n\"Load Map...\" and \"Save Map...\" allow you to do just that.\n\"Generate...\" allows you to randomly generate specific numbers of powerups (et al.) in the level.\n\"Clear Map...\", of course, clears the map.\n\"Item Spawn Time in s\" is the number of seconds (0 to disable or 1-65535 seconds) after which it checks whether to spawn a powerup into the level.\n\"% Chance to Spawn\" is the chance (calculated at every check time) it'll actually spawn a powerup (0-100).\nThe table under \"Relative Chance to Spawn\" allows you to specify, relatively (0-255 for each), the frequency of each powerup that spawns in (each powerup has a chance, if the level decides it's time to spawn an item, equal to its specified number divided by the total of all numbers in the table).\n\nLastly, press the \"Keys...\" button to edit the keys you'll be using to play.\n\nIt's simple!", "DXBomns Help", MB_OK | MB_ICONINFORMATION);
					return TRUE;

				case ID_KEYS: //go to keys dialog (see near the bottom of this file)
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_KEYS), hWndDlg, (DLGPROC)KeysDialogProc);
					return TRUE;

				case IDOK:
				case IDCANCEL:
				case ID_EDIT:
				{
					APPOPTS ao; //if we're leaving this dialog, save the current edittext to the .dat files
					ao.yFPS = (SendMessage(GetDlgItem(hWndDlg, IDC_APP_FPS), BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0);
					ao.yAudio = (SendMessage(GetDlgItem(hWndDlg, IDC_APP_AUDIO), BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0);
					SetAppOpts(&ao);

					GAMOPTS go; //for all of 'em
					go.yPlayers = GetDlgItemInt(hWndDlg, IDC_GAM_NPLAYERS, NULL, FALSE);
					go.yStartHealth = GetDlgItemInt(hWndDlg, IDC_GAM_STARTHEALTH, NULL, FALSE);
					go.yStartBomns = GetDlgItemInt(hWndDlg, IDC_GAM_STARTBOMNS, NULL, FALSE);
					go.yStartRadius = GetDlgItemInt(hWndDlg, IDC_GAM_STARTRADIUS, NULL, FALSE);
					go.yStartDamage = GetDlgItemInt(hWndDlg, IDC_GAM_STARTDAMAGE, NULL, FALSE);
					go.yMaxHealth = GetDlgItemInt(hWndDlg, IDC_GAM_MAXHEALTH, NULL, FALSE);
					go.yMaxBomns = GetDlgItemInt(hWndDlg, IDC_GAM_MAXBOMNS, NULL, FALSE);
					go.yMaxRadius = GetDlgItemInt(hWndDlg, IDC_GAM_MAXRADIUS, NULL, FALSE);
					go.yMaxDamage = GetDlgItemInt(hWndDlg, IDC_GAM_MAXDAMAGE, NULL, FALSE);
					go.yMinRadius = GetDlgItemInt(hWndDlg, IDC_GAM_MINRADIUS, NULL, FALSE);
					go.yMinDamage = GetDlgItemInt(hWndDlg, IDC_GAM_MINDAMAGE, NULL, FALSE);
					go.wTimeLimit = GetDlgItemInt(hWndDlg, IDC_GAM_TIMELIMIT, NULL, FALSE);
					go.yMaxBomnsOut = GetDlgItemInt(hWndDlg, IDC_GAM_MAXBOMNSOUT, NULL, FALSE);
					go.yInvulTime = GetDlgItemInt(hWndDlg, IDC_GAM_INVULTIME, NULL, FALSE);
					go.yBomnTimer = GetDlgItemInt(hWndDlg, IDC_GAM_BOMNTIMER, NULL, FALSE);
					go.yHealthPerPowerup = GetDlgItemInt(hWndDlg, IDC_GAM_HEALTHPERPOWERUP, NULL, FALSE);
					go.yPunchDamage = GetDlgItemInt(hWndDlg, IDC_GAM_PUNCHDAMAGE, NULL, FALSE);
					go.yPersistentPowerups = (SendMessage(GetDlgItem(hWndDlg, IDC_GAM_PERSISTENTPOWERUPS), BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0);
					SetGamOpts(&go);

					LEVOPTS lo;
					lo.wSpawnFreq = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNFREQ, NULL, FALSE);
					lo.ySpawnChance = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNCHANCE, NULL, FALSE);
					lo.ySpawnBomns = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNBOMNS, NULL, FALSE);
					lo.ySpawnHealth = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNHEALTH, NULL, FALSE);
					lo.ySpawnInvul = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNINVUL, NULL, FALSE);
					lo.ySpawnRadUp = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNRADUPS, NULL, FALSE);
					lo.ySpawnRadDown = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNRADDOWNS, NULL, FALSE);
					lo.ySpawnDamUp = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNDAMUPS, NULL, FALSE);
					lo.ySpawnDamDown = GetDlgItemInt(hWndDlg, IDC_LEV_SPAWNDAMDOWNS, NULL, FALSE);
					SetLevOpts(&lo);

					if(pLevel) //dump the level
						pLevel->DumpToFile(".\\lev.dat");

					DestroyWindow(g_hCurrentWnd); //destroy it
					if(LOWORD(wParam) == IDCANCEL) //quit, if they pressed quit
						PostQuitMessage(0);
					else if(LOWORD(wParam) == IDOK) //go to game if they pressed play
						EnterGame();
					else if(LOWORD(wParam) == ID_EDIT) //or, go to editor if they pressed edit
						EnterEditor();
					return TRUE;
				}

				case ID_LOAD:
				case ID_SAVE:
					if(pLevel)
					{
						CHAR szFile[2048] = ""; //set up the openfilename struct
						OPENFILENAME ofn =
						{
							sizeof(OPENFILENAME),
							hWndDlg, NULL,
							"DXBomns levels (*.lev)\0*.lev\0All files (*.*)\0*.*\0",
							NULL, 0, 0,
							szFile, 2048,
							NULL, 0,
							".",
							(LOWORD(wParam) == ID_LOAD ? "Open DXBomns Level" : "Save DXBomns Level As"),
							OFN_HIDEREADONLY | OFN_NOCHANGEDIR | (LOWORD(wParam) == ID_LOAD ? OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST : OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT),
							0, 0,
							"lev",
							0, NULL, NULL
						};

						if(LOWORD(wParam) == ID_LOAD) //if they pressed load, load it
						{
							if(GetOpenFileName(&ofn))
							{
								pLevel->LoadFromFile(szFile);
								pLevel->GeneratePreview();
								DeleteObject(hBmpPreview);    //and re-get preview
								hBmpPreview = (HBITMAP)LoadImage(NULL, ".\\preview.dat", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
								InvalidateRect(GetDlgItem(hWndDlg, ID_EDIT), NULL, FALSE); //make sure to draw it
							}
						}
						else
						{
							if(GetSaveFileName(&ofn)) //save it
							{
								pLevel->DumpToFile(szFile);
							}
						}
					}
					return TRUE;

				case ID_GEN:
				{
					switch(DialogBox(g_hInst, MAKEINTRESOURCE(IDD_GEN), hWndDlg, (DLGPROC)GenDialogProc))
					{
						case 0: //they pressed cancel
							break;

						case 1: //they pressed generate add'l
							if(pLevel)
							{
								pLevel->Generate();
								pLevel->GeneratePreview();
								DeleteObject(hBmpPreview); //generate, and reload preview
								hBmpPreview = (HBITMAP)LoadImage(NULL, ".\\preview.dat", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
								InvalidateRect(GetDlgItem(hWndDlg, ID_EDIT), NULL, FALSE);
							}
							break;

						case 2: //generate from scratch
							if(pLevel)
							{
								pLevel->Clear();
								pLevel->Generate();
								pLevel->GeneratePreview();
								DeleteObject(hBmpPreview);
								hBmpPreview = (HBITMAP)LoadImage(NULL, ".\\preview.dat", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
								InvalidateRect(GetDlgItem(hWndDlg, ID_EDIT), NULL, FALSE);
							}
							break;
					}
					return TRUE;
				}

				case ID_CLEAR:
					if(pLevel)
					{
						if(MessageBox(hWndDlg, "Are you sure you wish to clear the level?", "DXBomns", MB_YESNO | MB_ICONQUESTION) == IDYES)
						{
							pLevel->Clear();
							pLevel->GeneratePreview();
							DeleteObject(hBmpPreview); //blah
							hBmpPreview = (HBITMAP)LoadImage(NULL, ".\\preview.dat", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
							InvalidateRect(GetDlgItem(hWndDlg, ID_EDIT), NULL, FALSE);
						}
					}
					return TRUE;
			}
			break;

		case WM_DRAWITEM:
			if(wParam == ID_EDIT)
			{
				PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;
				
				DrawEdge(pdis->hDC, &pdis->rcItem, (pdis->itemState & ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED), BF_RECT); //draw the border around it

				HDC hBitmapDC = CreateCompatibleDC(pdis->hDC);
				SelectObject(hBitmapDC, hBmpPreview);
				BitBlt(pdis->hDC, 2, 2, 322, 227, hBitmapDC, 0, 0, SRCCOPY); //draw the bitmap
				DeleteDC(hBitmapDC);

				if(pdis->itemState & ODS_FOCUS) //draw the focus rect
				{
					RECT rcDeal = {5, 5, 319, 224};
					DrawFocusRect(pdis->hDC, &rcDeal);
				}
				return TRUE;
			}
			break;

		case WM_DESTROY:
			if(hBmpPreview)
			{
				DeleteObject(hBmpPreview); //clean up
				hBmpPreview = NULL;
			}
			SAFEDELETE(pLevel)
			break;
	}
	
	return FALSE;
}

//window proc for the generate dialog
INT_PTR CALLBACK GenDialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			GENOPTS go;
			GetGenOpts(&go); //load stuff on entry
			SetDlgItemInt(hWndDlg, IDC_GEN_SEED, go.dwSeed, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_NORMALWALLS, go.wNormalWalls, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_INDESTWALLS, go.wIndestWalls, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_WARPS, go.wWarps, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_BOMNS, go.wBomns, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_HEALTHS, go.wHealths, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_INVULS, go.wInvuls, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_RADUPS, go.wRadUps, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_RADDOWNS, go.wRadDowns, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_DAMUPS, go.wDamUps, FALSE);
			SetDlgItemInt(hWndDlg, IDC_GEN_DAMDOWNS, go.wDamDowns, FALSE);
			return TRUE;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDCANCEL:
					EndDialog(hWndDlg, 0); //don't do anything
					return TRUE;

				case ID_CLEARGEN:
				case IDOK:
				{
					GENOPTS go;
					go.dwSeed = GetDlgItemInt(hWndDlg, IDC_GEN_SEED, NULL, FALSE);
					go.wNormalWalls = GetDlgItemInt(hWndDlg, IDC_GEN_NORMALWALLS, NULL, FALSE);
					go.wIndestWalls = GetDlgItemInt(hWndDlg, IDC_GEN_INDESTWALLS, NULL, FALSE);
					go.wWarps = GetDlgItemInt(hWndDlg, IDC_GEN_WARPS, NULL, FALSE);
					go.wBomns = GetDlgItemInt(hWndDlg, IDC_GEN_BOMNS, NULL, FALSE);
					go.wHealths = GetDlgItemInt(hWndDlg, IDC_GEN_HEALTHS, NULL, FALSE);
					go.wInvuls = GetDlgItemInt(hWndDlg, IDC_GEN_INVULS, NULL, FALSE);
					go.wRadUps = GetDlgItemInt(hWndDlg, IDC_GEN_RADUPS, NULL, FALSE);
					go.wRadDowns = GetDlgItemInt(hWndDlg, IDC_GEN_RADDOWNS, NULL, FALSE);
					go.wDamUps = GetDlgItemInt(hWndDlg, IDC_GEN_DAMUPS, NULL, FALSE);
					go.wDamDowns = GetDlgItemInt(hWndDlg, IDC_GEN_DAMDOWNS, NULL, FALSE);
					SetGenOpts(&go);

					EndDialog(hWndDlg, (LOWORD(wParam) == IDOK ? 1 : 2)); //return 1 if they pressed generate add'l, or 2 if they pressed gen from scratch
					return TRUE;
				}

				case ID_HELP:
					MessageBox(hWndDlg, "The level can't hold more than 2880 items total, and at least 4 spaces must be free of walls and warps (though the entire level may be full of powerups).  The level won't get messed up if you try to generate more than it'll hold, but the results can be unpredictable.  Keep your numbers reasonable (under 2880).", "DXBomns Level Generation", MB_OK | MB_ICONINFORMATION);
					return TRUE;
			}
			break;
	}

	return FALSE;
}

//window proc for the key config dialog
INT_PTR CALLBACK KeysDialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			CHAR szKey[1024]; //geez, this is a lot of code to just load the keys and set the edit text
			GetPrivateProfileString("player 1", "up", "up", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P1UP, szKey);
			GetPrivateProfileString("player 1", "down", "down", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P1DOWN, szKey);
			GetPrivateProfileString("player 1", "left", "left", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P1LEFT, szKey);
			GetPrivateProfileString("player 1", "right", "right", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P1RIGHT, szKey);
			GetPrivateProfileString("player 1", "drop", "rcontrol", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P1DROP, szKey);

			GetPrivateProfileString("player 2", "up", "w", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P2UP, szKey);
			GetPrivateProfileString("player 2", "down", "s", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P2DOWN, szKey);
			GetPrivateProfileString("player 2", "left", "a", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P2LEFT, szKey);
			GetPrivateProfileString("player 2", "right", "d", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P2RIGHT, szKey);
			GetPrivateProfileString("player 2", "drop", "lshift", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P2DROP, szKey);

			GetPrivateProfileString("player 3", "up", "i", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P3UP, szKey);
			GetPrivateProfileString("player 3", "down", "k", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P3DOWN, szKey);
			GetPrivateProfileString("player 3", "left", "j", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P3LEFT, szKey);
			GetPrivateProfileString("player 3", "right", "l", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P3RIGHT, szKey);
			GetPrivateProfileString("player 3", "drop", "space", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P3DROP, szKey);

			GetPrivateProfileString("player 4", "up", "numpad8", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P4UP, szKey);
			GetPrivateProfileString("player 4", "down", "numpad5", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P4DOWN, szKey);
			GetPrivateProfileString("player 4", "left", "numpad4", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P4LEFT, szKey);
			GetPrivateProfileString("player 4", "right", "numpad6", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P4RIGHT, szKey);
			GetPrivateProfileString("player 4", "drop", "numpad0", szKey, 1024, ".\\keys.cfg");
			SetDlgItemText(hWndDlg, IDC_KEY_P4DROP, szKey);
			return TRUE;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDCANCEL:
					EndDialog(hWndDlg, 0);
					return TRUE;

				case ID_HELP:
					MessageBox(hWndDlg, "The following keys are defined (and case insensitive):\na - z, 0 - 9, numpad0 - numpad9, f1 - f12,\n` (the tilde key), -, =, backspace, tab, [, ], \\,\ncapslock, ;, ' (apostrophe), enter, , (comma), ., /,\nlshift, rshift, lcontrol, rcontrol, lalt, ralt (left and right shift, control, and alt),\nlwin, rwin (left and right windows keys), appmenu (that other windows key),\nspace, left, right, up, down (arrow keys), insert, delete, home, end, pageup, pagedown,\nnumlock, numpad/, numpad*, numpad-, numpad+, numpad., numpadenter (the numpad keys).\n\nI think that's all of 'em!  So, if you wanted to bind the period key to something, just type \".\" in the box.  Or for, say, the space bar, type \"space\".  And don't be fooled by my parentheses... so if you wanted to bind the comma key, just type \",\" in the box.  It's easy.\n\nAlso, please note that if you type in something that's not a valid key, you leave one blank, or you have duplicate entries, the results are undefined.  I do my best to error check and make things still function correctly, but don't push it.", "DXBomns Keys", MB_OK | MB_ICONINFORMATION);
					return TRUE;

				case IDOK:
				{
					CHAR szKey[1024];
					//maybe check these somehow? ... nah
					GetDlgItemText(hWndDlg, IDC_KEY_P1UP, szKey, 1024); //save all the keys to the file
					WritePrivateProfileString("player 1", "up", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P1DOWN, szKey, 1024);
					WritePrivateProfileString("player 1", "down", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P1LEFT, szKey, 1024);
					WritePrivateProfileString("player 1", "left", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P1RIGHT, szKey, 1024);
					WritePrivateProfileString("player 1", "right", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P1DROP, szKey, 1024);
					WritePrivateProfileString("player 1", "drop", szKey, ".\\keys.cfg");

					GetDlgItemText(hWndDlg, IDC_KEY_P2UP, szKey, 1024);
					WritePrivateProfileString("player 2", "up", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P2DOWN, szKey, 1024);
					WritePrivateProfileString("player 2", "down", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P2LEFT, szKey, 1024);
					WritePrivateProfileString("player 2", "left", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P2RIGHT, szKey, 1024);
					WritePrivateProfileString("player 2", "right", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P2DROP, szKey, 1024);
					WritePrivateProfileString("player 2", "drop", szKey, ".\\keys.cfg");

					GetDlgItemText(hWndDlg, IDC_KEY_P3UP, szKey, 1024);
					WritePrivateProfileString("player 3", "up", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P3DOWN, szKey, 1024);
					WritePrivateProfileString("player 3", "down", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P3LEFT, szKey, 1024);
					WritePrivateProfileString("player 3", "left", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P3RIGHT, szKey, 1024);
					WritePrivateProfileString("player 3", "right", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P3DROP, szKey, 1024);
					WritePrivateProfileString("player 3", "drop", szKey, ".\\keys.cfg");

					GetDlgItemText(hWndDlg, IDC_KEY_P4UP, szKey, 1024);
					WritePrivateProfileString("player 4", "up", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P4DOWN, szKey, 1024);
					WritePrivateProfileString("player 4", "down", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P4LEFT, szKey, 1024);
					WritePrivateProfileString("player 4", "left", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P4RIGHT, szKey, 1024);
					WritePrivateProfileString("player 4", "right", szKey, ".\\keys.cfg");
					GetDlgItemText(hWndDlg, IDC_KEY_P4DROP, szKey, 1024);
					WritePrivateProfileString("player 4", "drop", szKey, ".\\keys.cfg");

					EndDialog(hWndDlg, 1);
					return TRUE;
				}
			}
			break;
	}

	return FALSE;
}