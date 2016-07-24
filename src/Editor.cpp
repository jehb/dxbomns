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
// Editor.cpp -- handles all the full-screen map editor code.  Pretty simple //
// stuff.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <mmsystem.h>

#include "DXBomns.h"
#include "EditorDialog.h"
#include "XNotDirect.h"
#include "DXWindow.h"
#include "Graphics.h"
#include "Input.h"
#include "Sound.h"
#include "Level.h"
#include "Font.h"
#include "Editor.h"


extern HWND                 g_hCurrentWnd;  //from DXBomns.cpp
extern BOOL              (* g_pfnUpdate)(); //also

extern CGraphics          * g_pGraphics;    //from XNotDirect.cpp
extern CSoundSystem       * g_pSoundSystem; // .
extern CInput             * g_pInput;       // .
extern CDevice            * g_pdKeyboard;   // .
extern CDevice            * g_pdMouse;      // .

APPOPTS                     g_aoEditor;     //for fps counter

BOOL                        g_bLeaveEditor    = FALSE;  //whether it should destroy things next frame
BOOL                        g_bEditorActive   = TRUE;   //active/inactive window

CImage                    * g_piSprites       = NULL;   //the sprites bitmap
CImage                    * g_piCursorBox     = NULL;   //cursor box bitmap
CImage                    * g_piBar           = NULL;   //blue bar at bottom

CSound                    * g_apsEdSound[NUM_ED_SOUNDS]; //all the sounds

CFont                     * g_pEditorFont     = NULL;   //to draw text

CLevel                    * g_pLevel          = NULL;   //what we're workin' with
BOOL                        g_bModified       = FALSE;  //whether it needs to be saved

PROMPT_TYPE                 g_ptPrompt        = PT_NONE; //what prompt is on screen (save, quit, etc.)
BOOL                        g_bEditingPlayers = FALSE;   //whether we're editing player starts vs. level tiles

FLOAT                       g_fSensitivity    = 1.0f;           //mouse sensitivity
INT                         g_nMouseX         = SCR_WIDTH / 2;  //mouse x
INT                         g_nMouseY         = SCR_HEIGHT / 2; //y
INT                         g_nTool           = 1;              //which tool we're using
INT                         g_nSwitchTool     = 1;              //which tool we just WERE using
INT                         g_nBrushSize      = 1;              //brush size
INT                         g_nSwitchBrushSize= 1;              //old brush size
BOOL                        g_bPlacing        = FALSE;          //left mouse button is down
BOOL                        g_bErasing        = FALSE;          //right mouse button is down


//goes into the full-screen map editor.  Destroy g_hCurrentWnd
//before calling this.  Returns whether it was successful
BOOL EnterEditor()
{
	g_bLeaveEditor = FALSE;     //reset all the deals
	g_bEditorActive = TRUE;
	g_bModified = FALSE;
	g_ptPrompt = PT_NONE;
	g_bEditingPlayers = FALSE;
	g_nMouseX = SCR_WIDTH / 2;
	g_nMouseY = SCR_HEIGHT / 2;
	g_nTool = 1;
	g_nSwitchTool = 1;
	g_nBrushSize = 1;
	g_nSwitchBrushSize = 1;
	g_bPlacing = FALSE;
	g_bErasing = FALSE;

	if(!GrabOpts()) //get the newest options from the files
	{
		ExitEditor();
		return FALSE;
	}
	GetAppOpts(&g_aoEditor); //and grab the app opts

	if(!CreateDXWindow(ShutdownEditor, ActivateEditor)) //go into the DX window
	{
		ExitEditor();
		return FALSE;
	}

	if(!InitDX(EditorKeyInput, EditorMouseInput)) //initialize DX
	{
		ExitEditor();
		return FALSE;
	}
	
	g_piSprites = g_pGraphics->CreateImage(".\\gfx\\sprites.bmp", 0, 0, FALSE, 0); //create sprites
	if(!g_piSprites)
	{
		ExitEditor();
		return FALSE;
	}
	g_piCursorBox = g_pGraphics->CreateImage(".\\gfx\\cursorbox.bmp", 0, 0, TRUE, 0xf81f); //the cursor box
	if(!g_piCursorBox)
	{
		ExitEditor();
		return FALSE;
	}
	g_piBar = g_pGraphics->CreateImage(NULL, SCR_WIDTH, 48, FALSE, 0); //and the blue bar (it'll be cleared to blue in the update function)
	if(!g_piBar)
	{
		ExitEditor();
		return FALSE;
	}

	for(INT i = 0; i < NUM_ED_SOUNDS; i++) //set up the sounds
		g_apsEdSound[i] = NULL;
	for(i = 0; i < NUM_ED_SOUNDS; i++)
	{
		g_apsEdSound[i] = g_pSoundSystem->CreateSound(g_cacszEdSoundFile[i]);
		if(!g_apsEdSound[i])
		{
			ExitEditor();
			return FALSE;
		}
	}

	g_pEditorFont = CreateFont(g_pGraphics); //create a font
	if(!g_pEditorFont)
	{
		ExitEditor();
		return FALSE;
	}

	g_pLevel = CreateLevel(LT_EDIT, g_pGraphics, g_piSprites, g_pSoundSystem); //and the level
	if(!g_pLevel)
	{
		ExitEditor();
		return FALSE;
	}
	if(!g_pLevel->LoadFromFile(".\\lev.dat")) //load it
	{
		ExitEditor();
		return FALSE;
	}

	g_pfnUpdate = UpdateEditor; //tell DXBomns to call UpdateEditor each frame

	return TRUE;
}

//tells the editor to shut itself down next frame (to avoid deallocating things
//while they're being updated)
VOID ShutdownEditor()
{
	g_bLeaveEditor = TRUE;
}

//actually deallocates everything
VOID ExitEditor()
{
	g_pfnUpdate = NULL; //yeah
	
	SAFEDELETE(g_pLevel)
	SAFEDELETE(g_pEditorFont)

	for(INT i = 0; i < NUM_ED_SOUNDS; i++)
		SAFEDELETE(g_apsEdSound[i])

	SAFEDELETE(g_piSprites)
	SAFEDELETE(g_piCursorBox)
	SAFEDELETE(g_piBar)

	CloseDX();

	DestroyWindow(g_hCurrentWnd);
	EnterEditorDialog();	 //always go back to the editor dialog from the editor
}

//called when the app goes active/inactive
VOID ActivateEditor(BOOL bActive)
{
	if(g_bLeaveEditor) //if we're leaving anyway, don't do shit
		return;

	g_bEditorActive = bActive; //set it
	if(!g_bEditorActive)
	{
		//something?
	}
}

//called when someone presses a key
//returns whether it succeeds
BOOL EditorKeyInput(PINPUT_EVENT pie)
{
	if((BOOL)pie->lStatus) //if it's being pressed
	{
		if(pie->dwEvent == KEY_ESCAPE) //escape
		{
			if(g_ptPrompt == PT_NONE) //if there's no prompt
			{
				if(g_bModified && g_pLevel->IsValid()) //if it's valid and has been modified
					g_ptPrompt = PT_EXIT_SAVE;
				else if(g_bModified && !g_pLevel->IsValid()) //if it's modified but not valid
					g_ptPrompt = PT_EXIT_NOSAVE;
				else                                        //if it's not modified
					g_ptPrompt = PT_EXIT;

				if(!g_apsEdSound[SND_ED_PROMPT]->Play(FALSE)) //play the prompt sound
					return FALSE;
			}
			else                     //if there WAS a prompt
				g_ptPrompt = PT_NONE; //just clear it
		}
		else if(pie->dwEvent == KEY_SYSRQ) //print screen
		{
			if(!g_pGraphics->ScreenShot())
				return FALSE;
		}
		else if(g_ptPrompt != PT_NONE && pie->dwEvent == KEY_RETURN) //if there's a prompt and they press enter
		{
			switch(g_ptPrompt)
			{
				case PT_EXIT:
				case PT_EXIT_SAVE:
				case PT_EXIT_NOSAVE: //nothing to do but request exit
					ShutdownEditor();
					break;
				
				case PT_SAVE:  //save it
					if(!Save())
						return FALSE;
					break;

				case PT_LOAD:  //load it
					if(!Load())
						return FALSE;
					break;

				case PT_CLEAR: //clear it
					Clear();
					break;
			}

			g_ptPrompt = PT_NONE; //and no more prompt
		}
		else if(g_ptPrompt == PT_EXIT_SAVE && pie->dwEvent == KEY_S) //alternately, save if they press S from PT_EXIT_SAVE
		{
			if(!Save())
				return FALSE;
			ShutdownEditor(); //and then shut it down
		}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD1) {if(!ChangeBrushSize( 1, TRUE)) return FALSE;} //change the brush size
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD2) {if(!ChangeBrushSize( 2, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD3) {if(!ChangeBrushSize( 3, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD4) {if(!ChangeBrushSize( 4, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD5) {if(!ChangeBrushSize( 5, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD6) {if(!ChangeBrushSize( 6, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD7) {if(!ChangeBrushSize( 7, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD8) {if(!ChangeBrushSize( 8, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD9) {if(!ChangeBrushSize( 9, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_NUMPAD0) {if(!ChangeBrushSize(10, TRUE)) return FALSE;}
		else if(pie->dwEvent == KEY_1)                       {if(!ChangeTool( 1, TRUE)) return FALSE;} //change the tool
		else if(pie->dwEvent == KEY_2)                       {if(!ChangeTool( 2, TRUE)) return FALSE;}
		else if(pie->dwEvent == KEY_3)                       {if(!ChangeTool( 3, TRUE)) return FALSE;}
		else if(pie->dwEvent == KEY_4)                       {if(!ChangeTool( 4, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_5) {if(!ChangeTool( 5, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_6) {if(!ChangeTool( 6, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_7) {if(!ChangeTool( 7, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_8) {if(!ChangeTool( 8, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_9) {if(!ChangeTool( 9, TRUE)) return FALSE;}
		else if(!g_bEditingPlayers && pie->dwEvent == KEY_0) {if(!ChangeTool(10, TRUE)) return FALSE;}
		else if(pie->dwEvent == KEY_GRAVE) {if(!SwitchModes()) return FALSE;} //switch modes
		else if(pie->dwEvent == KEY_MINUS)  g_fSensitivity *= 0.75f; //change sensitivity
		else if(pie->dwEvent == KEY_EQUALS) g_fSensitivity *= 1.5f;
		else if(g_ptPrompt == PT_NONE && pie->dwEvent == KEY_F1) {if(!g_apsEdSound[SND_ED_PROMPT]->Play(FALSE)) return FALSE; g_ptPrompt = PT_HELP;} //help
		else if(g_ptPrompt == PT_NONE && g_pLevel->IsValid() && pie->dwEvent == KEY_F2) {if(!g_apsEdSound[SND_ED_PROMPT]->Play(FALSE)) return FALSE; g_ptPrompt = PT_SAVE;} //save
		else if(g_ptPrompt == PT_NONE && pie->dwEvent == KEY_F3) {if(!g_apsEdSound[SND_ED_PROMPT]->Play(FALSE)) return FALSE; g_ptPrompt = PT_LOAD;} //load
		else if(g_ptPrompt == PT_NONE && pie->dwEvent == KEY_F4) {if(!g_apsEdSound[SND_ED_PROMPT]->Play(FALSE)) return FALSE; g_ptPrompt = PT_CLEAR;} //clear
		else
		{
			if(!g_apsEdSound[SND_ED_NO]->Play(FALSE)) //nope
				return FALSE;
		}
	}
	
	return TRUE;
}

//called whenever the mouse does something
//returns whether it was successful
BOOL EditorMouseInput(PINPUT_EVENT pie)//is yummy
{
	BOOL bMoved = FALSE; //we'll change this if we need to place or erase tiles

	if(pie->dwEvent == MOUSE_BUTT_0) //left button
	{
		g_bPlacing = (BOOL)pie->lStatus; //start or stop placing
		if(g_bPlacing)
			bMoved = TRUE; //place if we're starting to place
	}
	else if(pie->dwEvent == MOUSE_BUTT_1) //right button
	{
		g_bErasing = (BOOL)pie->lStatus; //start or stop erasing
		if(g_bErasing)
			bMoved = TRUE; //place if we're starting to erase
	}
	else if(pie->dwEvent == MOUSE_BUTT_2) //middle button
	{
		if((BOOL)pie->lStatus && !SwitchModes()) //switch modes
			return FALSE;
	}
	else if(pie->dwEvent == MOUSE_AXIS_X) //they moved it
	{
		g_nMouseX += (INT)(pie->lStatus * g_fSensitivity); //move it
		if(g_nMouseX < 32 - (g_nBrushSize << 4)) //clip it to screen edges
			g_nMouseX = 32 - (g_nBrushSize << 4);
		else if(g_nMouseX > SCR_WIDTH)
			g_nMouseX = SCR_WIDTH;
		bMoved = TRUE;
	}
	else if(pie->dwEvent == MOUSE_AXIS_Y) //same here
	{
		g_nMouseY += (INT)(pie->lStatus * g_fSensitivity);
		if(g_nMouseY < 32 - (g_nBrushSize << 4))
			g_nMouseY = 32 - (g_nBrushSize << 4);
		else if(g_nMouseY > SCR_HEIGHT - 48)
			g_nMouseY = SCR_HEIGHT - 48;
		bMoved = TRUE;
	}
	else if(!g_bEditingPlayers && pie->dwEvent == MOUSE_AXIS_Z && ((pie->lStatus > 0 && g_nBrushSize < 10) || (pie->lStatus < 0 && g_nBrushSize > 1)))
	{ //if they move the mouse wheel and aren't editing player starts
		if(pie->lStatus > 0) //brush size up
		{
			if(!ChangeBrushSize(g_nBrushSize + 1, TRUE))
				return FALSE;
		}
		else if(pie->lStatus < 0) //brush size down
		{
			if(!ChangeBrushSize(g_nBrushSize - 1, TRUE))
				return FALSE;
		}
	}
	else //nope
	{
		if(!g_apsEdSound[SND_ED_NO]->Play(FALSE))
			return FALSE;
	}

	if(bMoved && !Place()) //place it if it should be placed
		return FALSE;

	return TRUE;
}

//changes the brush size.  Plays a sound if bSound is true
//returns whether it succeeded
BOOL ChangeBrushSize(INT nNewSize, BOOL bSound)
{
	if(g_ptPrompt != PT_NONE) //only do it if there's no prompt
	{
		if(bSound && !g_apsEdSound[SND_ED_NO]->Play(FALSE))
			return FALSE;

		return TRUE;
	}

	g_nBrushSize = nNewSize;
	if(bSound && !g_apsEdSound[SND_ED_TOOL]->Play(FALSE))
		return FALSE;

	if(g_nMouseX < 32 - (g_nBrushSize << 4)) //re-clip the mouse
		g_nMouseX = 32 - (g_nBrushSize << 4);
	if(g_nMouseY < 32 - (g_nBrushSize << 4))
		g_nMouseY = 32 - (g_nBrushSize << 4);

	return TRUE;
}

//changes the current tool.  Plays a sound if bSound is true
//returns whether it was successful
BOOL ChangeTool(INT nNewTool, BOOL bSound)
{
	if(g_ptPrompt != PT_NONE) //only do it if no prompt
	{
		if(bSound && !g_apsEdSound[SND_ED_NO]->Play(FALSE))
			return FALSE;

		return TRUE;
	}

	g_nTool = nNewTool;
	if(bSound && !g_apsEdSound[SND_ED_TOOL]->Play(FALSE))
		return FALSE;

	return TRUE;
}

//saves old brush size and tool, switches modes
//returns whether it succeeds
BOOL SwitchModes()
{
	if(g_ptPrompt != PT_NONE) //don't do it if there's a prompt
	{
		if(!g_apsEdSound[SND_ED_NO]->Play(FALSE))
			return FALSE;

		return TRUE;
	}

	g_bEditingPlayers = !g_bEditingPlayers; //switch
	INT nTempTool = g_nTool;           //save old shits
	INT nTempBrushSize = g_nBrushSize;
	ChangeTool(g_nSwitchTool, FALSE);  //restore old shits
	ChangeBrushSize(g_nSwitchBrushSize, FALSE);
	g_nSwitchTool = nTempTool;
	g_nSwitchBrushSize = nTempBrushSize; //save 'em

	if(g_bEditingPlayers) //clip tool and brush size, if somehow the old one was invalid
	{
		if(g_nTool > 4)
			ChangeTool(1, FALSE);
		if(g_nBrushSize > 1)
			ChangeBrushSize(1, FALSE);
	}

	if(!g_apsEdSound[SND_ED_SWITCH]->Play(FALSE))
		return FALSE;

	return TRUE;
}

//places or erases tiles wherever the mouse is, if the right flags are set
//returns whether it succeeds
BOOL Place()
{
	if(!g_bPlacing && !g_bErasing) //if no flags are set, get out
		return TRUE;

	if(g_ptPrompt != PT_NONE) //if there's a prompt, get out
	{
		if(!g_apsEdSound[SND_ED_NO]->Play(FALSE))
			return FALSE;

		return TRUE;
	}

	BOOL bPlaced = FALSE; //didn't do it yet
	BOOL bErased = FALSE;

	if(g_bEditingPlayers) //do player start deals
	{
		INT nX = (g_nMouseX - 8) / 16; //get x and y coords
		INT nY = (g_nMouseY - 8) / 16;
		if(nX >= 0 && nX < LEVEL_WIDTH && nY >= 0 && nY < LEVEL_HEIGHT)//this can't be false, but it's ok
		{
			if(g_bPlacing && !g_bErasing) //if placing, not erasing
			{
				BYTE yOldX, yOldY;
				g_pLevel->GetPlayerStart(g_nTool - 1, &yOldX, &yOldY); //get the old one
				if(nX != (INT)yOldX || nY != (INT)yOldY) //if they're different, we placed something
					bPlaced = TRUE;
				g_pLevel->SetPlayerStart(g_nTool - 1, nX, nY); //place the new one
			}
			else if(!g_bPlacing && g_bErasing) //if erasing, not placing
			{
				for(INT i = 0; i < 4; i++) //for each player
				{
					BYTE yX, yY;
					g_pLevel->GetPlayerStart(i, &yX, &yY); //find the one that was there
					if(nX == (INT)yX && nY == (INT)yY)
					{
						g_pLevel->SetPlayerStart(i, 255, 255); //set it to an invalid position
						bErased = TRUE; //we erased something
					}
				}
			}
		}
	}
	else //not editing player starts
	{
		for(INT i = 0; i < g_nBrushSize; i++) //for each tile in the brush size
		{
			for(INT j = 0; j < g_nBrushSize; j++)
			{
				INT nX = (g_nMouseX - 8 + (j << 4)) / 16; //get the position
				INT nY = (g_nMouseY - 8 + (i << 4)) / 16;
				if(nX >= 0 && nX < LEVEL_WIDTH && nY >= 0 && nY < LEVEL_HEIGHT) //if it's on screen
				{
					UINT unOffset = MAKEOFFSET(nX, nY); //get the offset
					if(g_bPlacing && !g_bErasing) //if placing, not erasing
					{
						BYTE yOld = g_pLevel->GetTile(unOffset); //get old tile
						if(g_nTool != (INT)yOld) //if it's different, we placed something
							bPlaced = TRUE;
						g_pLevel->SetTile(unOffset, g_nTool); //set the new one
					}
					else if(!g_bPlacing && g_bErasing) //if erasing, not placing
					{
						BYTE yOld = g_pLevel->GetTile(unOffset); //get the old tile
						if(yOld) //if it was something, we erased it
							bErased = TRUE;
						g_pLevel->SetTile(unOffset, 0); //erase it
					}
				}
			}
		}
	}

	if(bPlaced) //if we placed something
	{
		g_bModified = TRUE;
		if(!g_apsEdSound[SND_ED_PLACE]->Play(FALSE))
			return FALSE;
	}
	else if(bErased) //if we erased something
	{
		g_bModified = TRUE;
		if(!g_apsEdSound[SND_ED_ERASE]->Play(FALSE))
			return FALSE;
	}

	return TRUE;
}

//saves the level to lev.dat
//returns whether it succeeds
BOOL Save()
{
	g_pLevel->DumpToFile(".\\lev.dat");
	if(!g_apsEdSound[SND_ED_DONE]->Play(FALSE))
		return FALSE;
	g_bModified = FALSE;
	return TRUE;
}

//loads the level from lev.dat
//returns whether it succeeds
BOOL Load()
{
	g_pLevel->LoadFromFile(".\\lev.dat");
	if(!g_apsEdSound[SND_ED_DONE]->Play(FALSE))
		return FALSE;
	g_bModified = FALSE;
	return TRUE;
}

//clears the level
//returns whether it succeeds
BOOL Clear()
{
	g_pLevel->Clear();
	if(!g_apsEdSound[SND_ED_DONE]->Play(FALSE))
		return FALSE;
	g_bModified = TRUE; //it doesn't ALWAYS modify it, but that's ok
	return TRUE;
}

//called each frame
//returns whether it succeeds
BOOL UpdateEditor()
{
	if(g_bLeaveEditor) //if the leave flag is set, dealloc and get out
	{
		ExitEditor();
		return TRUE;
	}

	if(!g_bEditorActive) //if it's inactive, don't do anything
		return TRUE;

	if(g_pGraphics->NeedsReset()) //reset things
	{
		if(!g_pGraphics->Reset())
			return FALSE;

		if(g_piSprites && !g_piSprites->Reset())
			return FALSE;
		if(g_piCursorBox && !g_piCursorBox->Reset())
			return FALSE;
		if(g_piBar && !g_piBar->Reset())
			return FALSE;

		if(g_pEditorFont && !g_pEditorFont->Reset())
			return FALSE;

		if(g_pLevel && !g_pLevel->Reset())
			return FALSE;
	}
	if(g_piBar && g_piBar->NeedsPainting()) //if the bar needs painting
	{
		if(!g_piBar->Clear(NULL, 0x0010)) //clear it to blue
			return FALSE;
		g_piBar->SetPaintFlag(FALSE); //don't paint it
	}

	if(!g_pdKeyboard->Update()) //update deals
		return FALSE;
	if(!g_pdMouse->Update())
		return FALSE;

	if(g_pLevel && !g_pLevel->Render(NULL)) //render level
		return FALSE;

	if(g_bEditingPlayers) //if editing player starts, blit them
	{
		for(INT i = 0; i < 4; i++)
		{
			BYTE yX, yY;
			g_pLevel->GetPlayerStart(i, &yX, &yY);
			INT nX = yX, nY = yY;
			if(nX < LEVEL_WIDTH && nY < LEVEL_HEIGHT)
			{
				RECT rcDest;
				rcDest.left   = nX << 4;
				rcDest.top    = nY << 4;
				rcDest.right  = rcDest.left + 16;
				rcDest.bottom = rcDest.top + 16;
				RECT rcSource;
				rcSource.left   = (i << 4) + 16;
				rcSource.top    = 0;
				rcSource.right  = rcSource.left + 16;
				rcSource.bottom = rcSource.top + 16;
				if(!g_pGraphics->Blit(NULL, &rcDest, 1, g_piSprites, &rcSource))
					return FALSE;
			}
		}
	}

	for(INT i = 0; i < g_nBrushSize; i++) //draw mouse cursor
	{
		for(INT j = 0; j < g_nBrushSize; j++)
		{
			RECT rcDest;
			rcDest.left   = g_nMouseX - 16 + (j << 4);
			rcDest.top    = g_nMouseY - 16 + (i << 4);
			rcDest.right  = rcDest.left + 16;
			rcDest.bottom = rcDest.top + 16;
			RECT rcSource;
			if(g_bEditingPlayers)
			{
				rcSource.left = g_nTool << 4;
				rcSource.top  = 0;
			}
			else
			{
				switch(g_nTool)
				{
					case 1: case 2: case 6: rcSource.left =  80; break;
					case 7: case 8: case 4: rcSource.left =  96; break;
					case 9: case 10:        rcSource.left = 112; break;
					default:                rcSource.left =   0; break;
				}
				switch(g_nTool)
				{
					case 5: case 2: case 8: case 10: rcSource.top = 16; break;
					case 3: case 6: case 4:          rcSource.top = 32; break;
					default:                         rcSource.top =  0; break;
				}
			}
			rcSource.right  = rcSource.left + 16;
			rcSource.bottom = rcSource.top + 16;

			if(!g_pGraphics->Blit(NULL, &rcDest, 1, g_piSprites, &rcSource))
				return FALSE;
		}
	}

	for(i = 0; i < g_nBrushSize + 2; i++) //draw the box around the cursor
	{
		for(INT j = 0; j < g_nBrushSize + 2; j++)
		{
			RECT rcDest;
			rcDest.left   = g_nMouseX - 32 + (j << 4);
			rcDest.top    = g_nMouseY - 32 + (i << 4);
			rcDest.right  = rcDest.left + 16;
			rcDest.bottom = rcDest.top + 16;
			RECT rcSource;
			rcSource.left   = (j == 0 ? 0 : (j == g_nBrushSize + 1 ? 2 : 1)) << 4;
			rcSource.top    = (i == 0 ? 0 : (i == g_nBrushSize + 1 ? 2 : 1)) << 4;
			rcSource.right  = rcSource.left + 16;
			rcSource.bottom = rcSource.top + 16;

			if(!g_pGraphics->Blit(NULL, &rcDest, 1, g_piCursorBox, &rcSource))
				return FALSE;
		}
	}

	RECT rcDest = {0, SCR_HEIGHT - 48, SCR_WIDTH, SCR_HEIGHT}; //blit bar
	if(!g_pGraphics->Blit(NULL, &rcDest, 1, g_piBar, NULL))
		return FALSE;

	if(!g_pEditorFont->Box(NULL, 0, SCR_HEIGHT - 48, 64, 3)) //and blit a box over it
		return FALSE;

	switch(g_ptPrompt)
	{
		case PT_NONE: break; //prompt messages
		case PT_EXIT: if(!g_pEditorFont->Draw(NULL, "Really exit?")) return FALSE; break;
		case PT_EXIT_SAVE: if(!g_pEditorFont->Draw(NULL, "Exit without saving?")) return FALSE; break;
		case PT_EXIT_NOSAVE: if(!g_pEditorFont->Draw(NULL, "You can't save the level at this time.\n      Do you still wish to exit?")) return FALSE; break;
		case PT_SAVE: if(!g_pEditorFont->Draw(NULL, "Save changes to the level?")) return FALSE; break;
		case PT_LOAD: if(!g_pEditorFont->Draw(NULL, "Discard changes and reload the level?")) return FALSE; break;
		case PT_CLEAR: if(!g_pEditorFont->Draw(NULL, "Clear the level?")) return FALSE; break;
		case PT_HELP: if(!g_pEditorFont->Draw(NULL, "Draw with the left mouse button.  Erase with the right.\nThe number keys change the current tool and the number\n pad keys (and the mouse wheel) change the brush size.\n Switch between editing the level and the player start\n positions with the ~ key or the middle mouse button.\n  F1 displays this message, F2 saves changes to the\n level, F3 discards changes and reloads the level, and\nF4 clears the level.  Adjust the mouse sensitivity with\n    - and =.  And, of course, press <ESC> to exit.\n\n  You won't be able to save the level until all four\n             players have a start position.\n\n\nIf you're confused about the different tools/powerups,\n  you'll have to play the game some to get a feel for\n    what they all do.  Experimentation is the key!")) return FALSE; break;
	}
	switch(g_ptPrompt) //draw the bar text at the bottom
	{
		case PT_NONE:
			if(g_pLevel->IsValid()) //if it's valid
			{
				if(!g_pEditorFont->Scribe(NULL, 96, SCR_HEIGHT - 32, "F1 = Help     F2 = Save     F3 = Load     F4 = Clear"))
					return FALSE;
			}
			else //not valid
			{
				if(!g_pEditorFont->Scribe(NULL, 96, SCR_HEIGHT - 32, "F1 = Help                   F3 = Load     F4 = Clear"))
					return FALSE;
			}
			if(g_bModified && !g_pEditorFont->Scribe(NULL, 2, SCR_HEIGHT - 46, "*")) //modified
				return FALSE;
			break;

		case PT_EXIT:
		case PT_EXIT_NOSAVE: //default yes/no
		case PT_SAVE:
		case PT_LOAD:
		case PT_CLEAR:
			if(!g_pEditorFont->Scribe(NULL, 288, SCR_HEIGHT - 32, "<ENTER> = Yes     <ESC> = No"))
				return FALSE;
			break;

		case PT_EXIT_SAVE: //yes/no/s
			if(!g_pEditorFont->Scribe(NULL, 184, SCR_HEIGHT - 32, "<ENTER> = Yes     <ESC> = No     S = Save"))
				return FALSE;
			break;

		case PT_HELP: //help
			if(!g_pEditorFont->Scribe(NULL, 248, SCR_HEIGHT - 32, "<ENTER> or <ESC> = Go away, help!"))
				return FALSE;
			break;
	}
	
	if(g_aoEditor.yFPS) //if we're to display fps
	{
		static DWORD dwCurrentTime = timeGetTime();
		static DWORD dwLastFPSUpdate = dwCurrentTime;
		static DWORD dwFrame = 0;
		static FLOAT fFPS = 0.0f;

		dwCurrentTime = timeGetTime(); //get time

		if(dwCurrentTime >= dwLastFPSUpdate + 1000) //only update each second
		{
			fFPS = (FLOAT)dwFrame / (FLOAT)(dwCurrentTime - dwLastFPSUpdate) * 1000.0f; //get fps
			dwFrame = 0;
			dwLastFPSUpdate = dwCurrentTime;
		}

		if(g_pEditorFont && !g_pEditorFont->Scribe(NULL, 0, 0, "%.1f fps", fFPS)) //write it
			return FALSE;

		dwFrame++; //next frame
	}

	if(!g_pGraphics->Flip()) //display the deals
		return FALSE;

	return TRUE;
}