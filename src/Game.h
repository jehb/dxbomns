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
// Game.h -- the header for the main game engine.  Check out the .cpp file   //
// for more info.                                                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __GAME_H__
#define __GAME_H__


#include "InputEx.h"


typedef enum _GAMESTATE //what state the game is in
{
	GS_INTRO,       //at intro screen
	GS_COUNTDOWN,   //counting down
	GS_INGAME,      //game runnin'
	GS_PAUSED,      //paused
	GS_WON_WAITING, //someone has won, we're just screwin' around for a few seconds
	GS_WINNER,      //at the winner screen

	GS_FORCE_DWORD = 0x7fffffff
} GAMESTATE, * PGAMESTATE;


BOOL EnterGame();               //go to game
VOID ShutdownGame();            //request game exit
VOID ExitGame();                //dealloc stuff
VOID Activate(BOOL);            //activate/inactivate
BOOL PauseGame();               //pause
BOOL KeyInput(PINPUT_EVENT);    //proc key input
BOOL MouseInput(PINPUT_EVENT);  //mouse input
BOOL UpdateGame();              //each frame


#endif