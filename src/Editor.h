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
// Editor.h -- header for the game's fullscreen map editor.  Check the .cpp  //
// for more info.                                                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __EDITOR_H__
#define __EDITOR_H__


#define SND_ED_PLACE	 0 //offsets in the sound array -- this is when they place a tile
#define SND_ED_ERASE  1 //when they erase a tile
#define SND_ED_PROMPT 2 //do you want to x?
#define SND_ED_DONE   3 //done saving/loading/clearing
#define SND_ED_NO     4 //can't do that
#define SND_ED_SWITCH 5 //switch between editing players/tiles
#define SND_ED_TOOL   6 //played when they change the tool
#define NUM_ED_SOUNDS 7 //how many sounds were there


const CHAR * const g_cacszEdSoundFile[NUM_ED_SOUNDS] = //file names for sound array
{
	".\\sfx\\ed-place.wav",
	".\\sfx\\ed-erase.wav",
	".\\sfx\\ed-prompt.wav",
	".\\sfx\\ed-done.wav",
	".\\sfx\\ed-no.wav",
	".\\sfx\\ed-switch.wav",
	".\\sfx\\ed-tool.wav"
};


typedef enum _PROMPT_TYPE //what prompt is on screen
{
	PT_NONE,       //no prompt
	PT_EXIT,       //they're exiting, but haven't changed the level (enter=ok, esc=cancel)
	PT_EXIT_SAVE,  //they're exiting, and can save the level (modified) (enter=quit w/o saving, s=save/quit, esc=cancel)
	PT_EXIT_NOSAVE,//they're exiting, but can't save (modified) (enter=quit w/o saving, esc=cancel)
	PT_SAVE,       //they're saving the level (enter=save, esc=cancel)
	PT_LOAD,       //they're loading a level (enter=load, esc=cancel)
	PT_CLEAR,      //clearing the level (enter=clear, esc=cancel)
	PT_HELP,       //help box (enter=esc=go away)

	PT_FORCE_DWORD = 0x7fffffff
} PROMPT_TYPE, * PPROMPT_TYPE;


BOOL EnterEditor();                  //go to editor
VOID ShutdownEditor();               //request editor
VOID ExitEditor();                   //clean up shit
VOID ActivateEditor(BOOL);           //activate/inactivate
BOOL EditorKeyInput(PINPUT_EVENT);   //on key input
BOOL EditorMouseInput(PINPUT_EVENT); //on mouse input
BOOL ChangeBrushSize(INT, BOOL);     //changes brush size
BOOL ChangeTool(INT, BOOL);          //tool
BOOL SwitchModes();                  //player starts/tiles
BOOL Place();                        //place a tile
BOOL Save();                         //save level
BOOL Load();                         //load
BOOL Clear();                        //clear
BOOL UpdateEditor();                 //each frame


#endif