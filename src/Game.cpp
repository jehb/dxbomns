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
// Game.cpp -- this controls it all.  Well, not really... it's the main game //
// engine that controls the world, which really controls it all.             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "DXBomns.h"
#include "EditorDialog.h"
#include "DXWindow.h"
#include "XNotDirect.h"
#include "Graphics.h"
#include "Sound.h"
#include "Input.h"
#include "Font.h"
#include "World.h"
#include "Opts.h"
#include "Ver.h"
#include "Game.h"


extern HWND                 g_hCurrentWnd;  //from DXBomns.cpp
extern BOOL              (* g_pfnUpdate)(); // .

extern CGraphics          * g_pGraphics;	  //from XNotDirect.cpp
extern CSoundSystem       * g_pSoundSystem; // .
extern CInput             * g_pInput;       // .
extern CDevice            * g_pdKeyboard;   // .
extern CDevice            * g_pdMouse;      // .

APPOPTS                     g_aoAppOpts;    //holds the opts for game.cpp
GAMOPTS                     g_goGameOpts;

BOOL                        g_bLeaveNow       = FALSE; //request to leave the game
BOOL                        g_bActive         = TRUE;  //active/inactive

GAMESTATE                   g_gsState         = GS_INTRO; //the actual game state (don't change directly)
GAMESTATE                   g_gsNewState      = GS_INTRO; //to change the game state, change this instead

CImage                    * g_piIntro         = NULL; //intro bitmap
CImage                    * g_piHud           = NULL; //hud bitmap
CImage                    * g_piBackground    = NULL; //holds the background at pivotal points (like when someone wins) so it doesn't have to be redrawn, and can have text over it

CSound                    * g_psWelcome       = NULL; //the sounds used herein
CSound                    * g_psTick          = NULL;
CSound                    * g_psStart         = NULL;
CSound                    * g_psPause         = NULL;
CSound                    * g_psUnpause       = NULL;
CSound                    * g_psWinner        = NULL;

CFont                     * g_pFont           = NULL; //the game's font

CWorld                    * g_pWorld          = NULL; //the world object
INT                         g_anKills[4]      = {0, 0, 0, 0}; //scores
INT                         g_anDeaths[4]     = {0, 0, 0, 0};
INT                         g_nWinner         = 0; //who's the winner?

DWORD                       g_dwGameStartTime = 0;   //when the game starts
DWORD                       g_dwStateChangeTime = 0; //when the state is supposed to change
DWORD                       g_dwNextTickTime  = 0;   //when the tick sound is supposed to play again
DWORD                       g_dwNextBlinkTime = 0;   //when the timer's supposed to blink
BOOL                        g_bDrawTime       = TRUE;//whether to draw timer
DWORD                       g_dwCurrentTime   = 0;   //current time, available to anyone
DWORD                       g_dwPausedTime    = 0;   //the time at which the game was paused (or the app went inactive)


//enters the game... creates a window, inits DX, etc.
//destroy g_hCurrentWnd before calling this
//returns whether it succeeds
BOOL EnterGame()
{
	g_bLeaveNow = FALSE; //set various stuff up
	g_bActive = TRUE;
	g_gsState = GS_INTRO;
	g_gsNewState = GS_INTRO;
	g_nWinner = 0;

	BOOL bSuccess = FALSE;
	do //this construct saves a LOT of space
	{
		if(!GrabOpts()) //get the opts from the files
			break;
		GetAppOpts(&g_aoAppOpts); //copy the app and gam opts
		GetGamOpts(&g_goGameOpts);

		if(!CreateDXWindow(ShutdownGame, Activate)) //create a dx window
			break;

		if(!InitDX(KeyInput, MouseInput)) //and init dx
			break;

		g_piIntro = g_pGraphics->CreateImage(".\\gfx\\intro.bmp", 0, 0, FALSE, 0); //create intro bitmap
		if(!g_piIntro)
			break;
		g_piHud = g_pGraphics->CreateImage(".\\gfx\\hud.bmp", 0, 0, FALSE, 0); //and hud bitmap
		if(!g_piHud)
			break;
		g_piBackground = g_pGraphics->CreateImage(NULL, SCR_WIDTH, SCR_HEIGHT, FALSE, 0); //create the background image
		if(!g_piBackground)
			break;

		g_psWelcome = g_pSoundSystem->CreateSound(".\\sfx\\welcome.wav"); //create all the sounds
		if(!g_psWelcome)
			break;
		g_psStart = g_pSoundSystem->CreateSound(".\\sfx\\start.wav");
		if(!g_psStart)
			break;
		g_psTick = g_pSoundSystem->CreateSound(".\\sfx\\tick.wav");
		if(!g_psTick)
			break;
		g_psPause = g_pSoundSystem->CreateSound(".\\sfx\\pause.wav");
		if(!g_psPause)
			break;
		g_psUnpause = g_pSoundSystem->CreateSound(".\\sfx\\unpause.wav");
		if(!g_psUnpause)
			break;
		g_psWinner = g_pSoundSystem->CreateSound(".\\sfx\\winner.wav");
		if(!g_psWinner)
			break;

		g_pFont = CreateFont(g_pGraphics); //create the font
		if(!g_pFont)
			break;

		g_pWorld = CreateWorld(g_pGraphics, g_pSoundSystem); //and the world
		if(!g_pWorld)
			break;

		if(!g_psWelcome->Play(FALSE)) //play the welcome wagon!
			break;

		bSuccess = TRUE;
	} while(0);

	if(!bSuccess) //if something didn't work, dealloc
	{
		ExitGame();
		return FALSE;
	}

	g_pfnUpdate = UpdateGame; //set the app to call this each frame

	return TRUE;
}

//request that next frame the game shuts down
//(so that nothing gets dealloc'd when in use)
VOID ShutdownGame()
{
	g_bLeaveNow = TRUE;
}

//actually deallocates all the shits
VOID ExitGame()
{
	g_pfnUpdate = NULL;

	SAFEDELETE(g_pWorld)

	SAFEDELETE(g_pFont)

	SAFEDELETE(g_psWelcome)
	SAFEDELETE(g_psTick)
	SAFEDELETE(g_psStart)
	SAFEDELETE(g_psPause)
	SAFEDELETE(g_psUnpause)
	SAFEDELETE(g_psWinner)

	SAFEDELETE(g_piIntro)
	SAFEDELETE(g_piHud)
	SAFEDELETE(g_piBackground)

	CloseDX();

	DestroyWindow(g_hCurrentWnd);
	EnterEditorDialog();
}

//called whenever the app goes active or inactive
VOID Activate(BOOL bActive)
{
	if(g_bLeaveNow) //don't worry about it if it's leaving anyway
		return;

	g_bActive = bActive;
	if(!g_bActive)
	{
		if(g_gsState == GS_COUNTDOWN) //go back to intro if they were counting down
			g_gsNewState = GS_INTRO;
		else if(g_gsState == GS_WON_WAITING) //and go straight to winner if they were waiting
			g_gsNewState = GS_WINNER;

		PauseGame(); //pause it
	}
}

//called when the game is supposed to be paused
//returns whether it succeeds
BOOL PauseGame()
{
	if(g_gsState == GS_PAUSED) //if it's already paused, don't do anything
		return TRUE;

	g_dwPausedTime = g_dwCurrentTime; //get the time

	if(g_gsState != GS_INGAME) //if it's not in the actual game, do nothing else
		return TRUE;

	g_gsNewState = GS_PAUSED; //pause the state

	if(g_pWorld)
		g_pWorld->PauseGame(); //and call pause for the world

	return TRUE;
}

//called when someone presses a key
//returns whether it succeeds
BOOL KeyInput(PINPUT_EVENT pie)
{
	if(pie->dwEvent == KEY_ESCAPE && (BOOL)pie->lStatus) //if they press escape
		ShutdownGame();
	else if(pie->dwEvent == KEY_SYSRQ && (BOOL)pie->lStatus) //print screen
	{
		if(!g_pGraphics->ScreenShot())
			return FALSE;
	}
	else if(pie->dwEvent == KEY_PAUSE && (BOOL)pie->lStatus) //pause
	{
		if(g_gsState == GS_PAUSED) //unpause it
			g_gsNewState = GS_INGAME;
		else if(g_gsState == GS_INGAME) //pause
		{
			if(!PauseGame())
				return FALSE;
		}
	}
	else if(g_gsState == GS_INTRO && (pie->dwEvent == KEY_RETURN || pie->dwEvent == KEY_SPACE) && (BOOL)pie->lStatus) //if they're in the intro screen and press enter or space
	{
		g_gsNewState = GS_COUNTDOWN; //go to countdown
	}
	else if((g_gsState == GS_INGAME || g_gsState == GS_WON_WAITING) && (BOOL)pie->lStatus) //otherwise, pass it to the world
	{
		if(!g_pWorld->KeyHit((BYTE)pie->dwEvent))
			return FALSE;
	}

	return TRUE;
}

//called when the mouse moves
//returns whether it succeeds
BOOL MouseInput(PINPUT_EVENT pie)
{
	if(pie->dwEvent == MOUSE_BUTT_0 && (BOOL)pie->lStatus) //left button
	{
		if(g_gsState == GS_INTRO)
			g_gsNewState = GS_COUNTDOWN; //go to countdown from intro
		else if(g_gsState == GS_WINNER)
			ShutdownGame();              //shut down game from winner screen
		else if(g_gsState == GS_PAUSED)
			g_gsNewState = GS_INGAME;    //unpause from pause
	}

	return TRUE;
}

//called each frame to update all the shits
//returns whether it succeeds
BOOL UpdateGame()
{
	g_dwCurrentTime = timeGetTime(); //ALWAYS get new time

	if(g_bLeaveNow) //if we're supposed to leave, do that
	{
		ExitGame();
		return TRUE;
	}
	
	if(!g_bActive) //now, if we're inactive, don't do anything
		return TRUE;

	if(g_gsState == g_gsNewState && g_dwCurrentTime >= g_dwStateChangeTime) //if it's time for a state change
	{
		if(g_gsState == GS_COUNTDOWN) //go to the new state
			g_gsNewState = GS_INGAME;
		else if(g_gsState == GS_INGAME && g_goGameOpts.wTimeLimit) //(only if there's a time limit)
			g_gsNewState = GS_WINNER;
		else if(g_gsState == GS_WON_WAITING)
			g_gsNewState = GS_WINNER;
	}

	//if we're not changing states and it's time for the timer to blink
	if(g_gsState == g_gsNewState && (g_gsState == GS_INGAME || g_gsState == GS_WON_WAITING) && g_dwCurrentTime >= g_dwNextBlinkTime)
	{
		g_dwNextBlinkTime += 500;   // do it
		g_bDrawTime = !g_bDrawTime;
	}
	
	if(g_gsState != g_gsNewState) //if we're changing states
	{
		if(g_gsState == GS_INTRO && g_gsNewState == GS_COUNTDOWN)//if going from intro to countdown
		{
			g_piBackground->SetPaintFlag(TRUE); //capture the background later on

			g_dwStateChangeTime = g_dwCurrentTime + 3000; //wait three seconds
			g_dwNextTickTime = g_dwCurrentTime; //tick now
		}
		else if(g_gsState == GS_COUNTDOWN && g_gsNewState == GS_INGAME)//from countdown to game
		{
			if(!g_psStart->Play(FALSE)) //play the start sound
				return FALSE;

			if(!g_pWorld->StartGame()) //start deals
				return FALSE;

			g_dwGameStartTime = g_dwCurrentTime; //get the times
			g_dwStateChangeTime = g_dwGameStartTime + g_goGameOpts.wTimeLimit * 1000;
			g_dwNextTickTime = g_dwStateChangeTime - (g_goGameOpts.wTimeLimit > 10 ? 10000 : (g_goGameOpts.wTimeLimit > 5 ? 5000 : 0));
			g_dwNextBlinkTime = g_dwGameStartTime + 500;
			g_bDrawTime = TRUE;
		}
		else if(g_gsState == GS_INGAME && g_gsNewState == GS_PAUSED)//pausing
		{
			g_piBackground->SetPaintFlag(TRUE); //capture the background later

			if(!g_psPause->Play(FALSE)) //play the pause sound
				return FALSE;
		}
		else if(g_gsState == GS_PAUSED && g_gsNewState == GS_INGAME)//unpausing
		{
			if(!g_psUnpause->Play(FALSE)) //play the sound
				return FALSE;
			g_pWorld->UnpauseGame(); //do it

			g_dwGameStartTime += g_dwCurrentTime - g_dwPausedTime; //add to all the timers
			g_dwStateChangeTime += g_dwCurrentTime - g_dwPausedTime;
			g_dwNextTickTime += g_dwCurrentTime - g_dwPausedTime;
			g_dwNextBlinkTime += g_dwCurrentTime - g_dwPausedTime;
		}
		else if(g_gsState == GS_INGAME && g_gsNewState == GS_WINNER)//going straight from game to winner screen (time is up)
		{
			if(!g_pWorld->Render(g_piBackground, FALSE, NULL)) //get background NOW
				return FALSE;
			if(!g_piBackground->Backup()) //back it up
				return FALSE;
			g_piBackground->SetPaintFlag(FALSE); //no more painting

			if(!g_psWinner->Play(FALSE))
				return FALSE;
		}
		else if(g_gsState == GS_INGAME && g_gsNewState == GS_WON_WAITING)//going into intermission (someone has won)
		{
			if(!g_pWorld->Render(g_piBackground, FALSE, NULL)) //capture background NOW
				return FALSE;
			if(!g_piBackground->Backup()) //back it up
				return FALSE;
			g_piBackground->SetPaintFlag(FALSE);

			g_dwStateChangeTime = g_dwCurrentTime + 3000; //set up timers
			g_dwNextTickTime = g_dwCurrentTime;
			g_dwNextBlinkTime = g_dwCurrentTime + 500;
			g_bDrawTime = TRUE;
		}
		else if(g_gsState == GS_WON_WAITING && g_gsNewState == GS_WINNER)//when going from intermission to winner screen (after intermission)
		{
			if(!g_psWinner->Play(FALSE))
				return FALSE;
		}
		else if(g_gsState == GS_COUNTDOWN && g_gsNewState == GS_INTRO)//if someone alt-tabbed during countdown and we're going back to intro
		{
		}

		g_gsState = g_gsNewState; //set the new state
	}

	if(g_pGraphics->NeedsReset()) //reset everything
	{
		if(!g_pGraphics->Reset())
			return FALSE;

		if(!g_piIntro->Reset())
			return FALSE;
		if(!g_piBackground->Reset())
			return FALSE;

		if(!g_pFont->Reset())
			return FALSE;
		
		if(!g_pWorld->Reset())
			return FALSE;
	}
	//if it's counting down or paused and the background needs to be painted
	if((g_gsState == GS_COUNTDOWN || g_gsState == GS_PAUSED) && g_piBackground && g_piBackground->NeedsPainting())
	{
		if(!g_pWorld->Render(g_piBackground, FALSE, NULL)) //do it
			return FALSE;
		g_piBackground->SetPaintFlag(FALSE); //no more painty
	}
	else if(g_gsState == GS_WINNER && g_piBackground && g_piBackground->NeedsPainting()) //or if it's at the winner screen (it will have been backed up before, so restore it)
	{
		if(!g_piBackground->RestoreFromBackup()) //restore
			return FALSE;
		g_piBackground->SetPaintFlag(FALSE); //no more painty
	}

	if(!g_pdKeyboard->Update()) //update deals
		return FALSE;
	if(!g_pdMouse->Update())
		return FALSE;

	if(g_dwCurrentTime >= g_dwNextTickTime) //tick tock
	{
		if(g_gsState == GS_COUNTDOWN || g_gsState == GS_WON_WAITING || (g_gsState == GS_INGAME && g_goGameOpts.wTimeLimit))
		{
			if(!g_psTick->Play(FALSE)) //play it if it's really time
				return FALSE;
			g_dwNextTickTime += 1000;
		}
	}

	switch(g_gsState)
	{
		case GS_INTRO: //if we're in the intro
			if(!g_pGraphics->Blit(NULL, NULL, 1, g_piIntro, NULL)) //blit the intro screen
				return FALSE;
			if(!g_pFont->Scribe(NULL, 0, SCR_HEIGHT - 16, DXBOMNS_VER_STR_SEMILONG)) //and draw the version
				return FALSE;
			break;

		case GS_INGAME: //if people can move around
		case GS_WON_WAITING:
		{
			if(!g_pWorld->Update()) //update deals
				return FALSE;
			if(g_gsState == GS_INGAME) //and if it's in the game,
			{
				g_nWinner = g_pWorld->Winner(); //look for a winner
				if(g_nWinner)
					g_gsNewState = GS_WON_WAITING; //if we found one, go to waiting
			}
			RECT rcDest = {0, SCR_HEIGHT - 48, SCR_WIDTH, SCR_HEIGHT};
			if(!g_pGraphics->Blit(NULL, &rcDest, 1, g_piHud, NULL)) //blit hud
				return FALSE;
			if(!g_pWorld->Render(NULL, TRUE, g_pFont)) //render to screen
				return FALSE;
			if((g_goGameOpts.wTimeLimit || g_gsState == GS_WON_WAITING) && g_bDrawTime && !g_pFont->Scribe(NULL, 472, 724, "%05d", (INT)ceil((g_dwStateChangeTime - g_dwCurrentTime) / 1000.0f)))
				return FALSE; //render time limit
			break;
		}

		case GS_COUNTDOWN: //otherwise
		case GS_PAUSED:
		case GS_WINNER:
			if(!g_pGraphics->Blit(NULL, NULL, 1, g_piBackground, NULL)) //blit the background
				return FALSE;
			if(g_gsState == GS_COUNTDOWN) //if it's counting down, display the timer
			{
				if(!g_pFont->Draw(NULL, "T minus %1.1f", (g_dwStateChangeTime - g_dwCurrentTime) / 1000.0f))
					return FALSE;
			}
			else if(g_gsState == GS_PAUSED) //if it's paused, display the pause info
			{
				CHAR szMessage[2048] = "               P A U S E D\n\n\n          Health | Bomns | Radius | Damage\n          -------|-------|--------|-------\n";
				if(g_goGameOpts.yPlayers)
				{
					for(INT i = 0; i < (INT)g_goGameOpts.yPlayers; i++)
					{
						INT nHealth = 0, nBomns = 0, nRad = 0, nDam = 0;
						g_pWorld->GetPlayerInfo(i + 1, &nHealth, &nBomns, &nRad, &nDam);

						if(nHealth > 0)
						{
							CHAR szTemp[256];
							sprintf(szTemp, "Player %d:     %2d |    %2d |     %2d | %2d\n", i + 1, nHealth, nBomns, nRad, nDam);
							strcat(szMessage, szTemp);
						}
						//that's a lot of info!
					}
					strcat(szMessage, "\n       Press <PAUSE> to resume game");
				}
				else
					strcpy(szMessage, "        P A U S E D\n\nPress <PAUSE> to resume game");

				if(!g_pFont->Draw(NULL, szMessage)) //draw the message
					return FALSE;
			}
			else if(g_gsState == GS_WINNER) //if it's winner screen
			{
				CHAR szWinMsg[128] = "         Tie!";
				if(g_nWinner != 5 && g_nWinner != 0)
					sprintf(szWinMsg,  "    Player %d wins!", g_nWinner); //winner message

				CHAR aszRatio[4][32];
				for(INT i = 0; i < 4; i++) //ratios
				{
					if(g_anDeaths[i])
						sprintf(aszRatio[i], "%5.3f", (FLOAT)g_anKills[i] / (FLOAT)g_anDeaths[i]);
					else if(g_anKills[i])
						sprintf(aszRatio[i], "infinite");
					else
						sprintf(aszRatio[i], "none yet");
				}
				
				if(!g_pFont->Draw(NULL, "       %s\n\n\n          Kills | Deaths | Ratio\n          ------|--------|----------\nPlayer 1: %5d | %5d  | %s\nPlayer 2: %5d | %5d  | %s\nPlayer 3: %5d | %5d  | %s\nPlayer 4: %5d | %5d  | %s\n\n        Press <Esc> to exit",
                              szWinMsg, g_anKills[0], g_anDeaths[0], aszRatio[0], g_anKills[1], g_anDeaths[1], aszRatio[1], g_anKills[2], g_anDeaths[2], aszRatio[2], g_anKills[3], g_anDeaths[3], aszRatio[3]))
					return FALSE; //draw the message
			}
			break;
	}

	if(g_aoAppOpts.yFPS) //if we're supposed to draw the FPS
	{
		static DWORD dwLastFPSUpdate = g_dwCurrentTime;
		static DWORD dwFrame = 0;
		static FLOAT fFPS = 0.0f;

		if(g_dwCurrentTime >= dwLastFPSUpdate + 1000) //only draw every second
		{
			fFPS = (FLOAT)dwFrame / (FLOAT)(g_dwCurrentTime - dwLastFPSUpdate) * 1000.0f;
			dwFrame = 0;
			dwLastFPSUpdate = g_dwCurrentTime; //update deals
		}

		if(!g_pFont->Scribe(NULL, 0, 0, "%.1f fps", fFPS))
			return FALSE;

		dwFrame++; //next frame
	}

	if(!g_pGraphics->Flip()) //flip back to front
		return FALSE;

	return TRUE;
}