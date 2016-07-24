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
// InputEx.h -- extra input stuff, not included in input.h for spacial       //
// reasons.  Actually, this mostly just defines the KEY_ constants.          //
// Anyway, look at the .cpp file for more info.                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __INPUTEX_H__
#define __INPUTEX_H__


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


typedef struct _INPUT_EVENT //the main focus of this file
{
	DWORD dwEvent; //where event occurred (device specific... i.e., a mouse event isn't a key event)
	LONG  lStatus; //what happened (BOOL == TRUE if key or button down FALSE if up; or for mouse axes, a LONG change in mickeys
	DWORD dwTime;  //timestamp that the event occurred at
} INPUT_EVENT, * PINPUT_EVENT;


/////////////////////////////////////////////////////////////
// DIK_ duplicates for the dwEvent field of an INPUT_EVENT //
// (use these instead of their DIK_ counterparts)          //
// note that a lot of this was copy/pasted from dinput.h   //
/////////////////////////////////////////////////////////////
#define KEY_ESCAPE          DIK_ESCAPE
#define KEY_1               DIK_1
#define KEY_2               DIK_2
#define KEY_3               DIK_3
#define KEY_4               DIK_4
#define KEY_5               DIK_5
#define KEY_6               DIK_6
#define KEY_7               DIK_7
#define KEY_8               DIK_8
#define KEY_9               DIK_9
#define KEY_0               DIK_0
#define KEY_MINUS           DIK_MINUS               /* - on main keyboard */
#define KEY_EQUALS          DIK_EQUALS
#define KEY_BACK            DIK_BACK                /* backspace */
#define KEY_TAB             DIK_TAB
#define KEY_Q               DIK_Q
#define KEY_W               DIK_W
#define KEY_E               DIK_E
#define KEY_R               DIK_R
#define KEY_T               DIK_T
#define KEY_Y               DIK_Y
#define KEY_U               DIK_U
#define KEY_I               DIK_I
#define KEY_O               DIK_O
#define KEY_P               DIK_P
#define KEY_LBRACKET        DIK_LBRACKET
#define KEY_RBRACKET        DIK_RBRACKET
#define KEY_RETURN          DIK_RETURN              /* Enter on main keyboard */
#define KEY_LCONTROL        DIK_LCONTROL
#define KEY_A               DIK_A
#define KEY_S               DIK_S
#define KEY_D               DIK_D
#define KEY_F               DIK_F
#define KEY_G               DIK_G
#define KEY_H               DIK_H
#define KEY_J               DIK_J
#define KEY_K               DIK_K
#define KEY_L               DIK_L
#define KEY_SEMICOLON       DIK_SEMICOLON
#define KEY_APOSTROPHE      DIK_APOSTROPHE
#define KEY_GRAVE           DIK_GRAVE               /* accent grave */
#define KEY_LSHIFT          DIK_LSHIFT
#define KEY_BACKSLASH       DIK_BACKSLASH
#define KEY_Z               DIK_Z
#define KEY_X               DIK_X
#define KEY_C               DIK_C
#define KEY_V               DIK_V
#define KEY_B               DIK_B
#define KEY_N               DIK_N
#define KEY_M               DIK_M
#define KEY_COMMA           DIK_COMMA
#define KEY_PERIOD          DIK_PERIOD              /* . on main keyboard */
#define KEY_SLASH           DIK_SLASH               /* / on main keyboard */
#define KEY_RSHIFT          DIK_RSHIFT
#define KEY_MULTIPLY        DIK_MULTIPLY            /* * on numeric keypad */
#define KEY_LMENU           DIK_LMENU               /* left Alt */
#define KEY_SPACE           DIK_SPACE
#define KEY_CAPITAL         DIK_CAPITAL
#define KEY_F1              DIK_F1
#define KEY_F2              DIK_F2
#define KEY_F3              DIK_F3
#define KEY_F4              DIK_F4
#define KEY_F5              DIK_F5
#define KEY_F6              DIK_F6
#define KEY_F7              DIK_F7
#define KEY_F8              DIK_F8
#define KEY_F9              DIK_F9
#define KEY_F10             DIK_F10
#define KEY_NUMLOCK         DIK_NUMLOCK
#define KEY_SCROLL          DIK_SCROLL              /* Scroll Lock */
#define KEY_NUMPAD7         DIK_NUMPAD7
#define KEY_NUMPAD8         DIK_NUMPAD8
#define KEY_NUMPAD9         DIK_NUMPAD9
#define KEY_SUBTRACT        DIK_SUBTRACT            /* - on numeric keypad */
#define KEY_NUMPAD4         DIK_NUMPAD4
#define KEY_NUMPAD5         DIK_NUMPAD5
#define KEY_NUMPAD6         DIK_NUMPAD6
#define KEY_ADD             DIK_ADD                 /* + on numeric keypad */
#define KEY_NUMPAD1         DIK_NUMPAD1
#define KEY_NUMPAD2         DIK_NUMPAD2
#define KEY_NUMPAD3         DIK_NUMPAD3
#define KEY_NUMPAD0         DIK_NUMPAD0
#define KEY_DECIMAL         DIK_DECIMAL             /* . on numeric keypad */
#define KEY_OEM_102         DIK_OEM_102             /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define KEY_F11             DIK_F11
#define KEY_F12             DIK_F12
#define KEY_F13             DIK_F13                 /*                     (NEC PC98) */
#define KEY_F14             DIK_F14                 /*                     (NEC PC98) */
#define KEY_F15             DIK_F15                 /*                     (NEC PC98) */
#define KEY_KANA            DIK_KANA                /* (Japanese keyboard)            */
#define KEY_ABNT_C1         DIK_ABNT_C1             /* /? on Brazilian keyboard */
#define KEY_CONVERT         DIK_CONVERT             /* (Japanese keyboard)            */
#define KEY_NOCONVERT       DIK_NOCONVERT           /* (Japanese keyboard)            */
#define KEY_YEN             DIK_YEN                 /* (Japanese keyboard)            */
#define KEY_ABNT_C2         DIK_ABNT_C2             /* Numpad . on Brazilian keyboard */
#define KEY_NUMPADEQUALS    DIK_NUMPADEQUALS        /* = on numeric keypad (NEC PC98) */
#define KEY_PREVTRACK       DIK_PREVTRACK           /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
#define KEY_AT              DIK_AT                  /*                     (NEC PC98) */
#define KEY_COLON           DIK_COLON               /*                     (NEC PC98) */
#define KEY_UNDERLINE       DIK_UNDERLINE           /*                     (NEC PC98) */
#define KEY_KANJI           DIK_KANJI               /* (Japanese keyboard)            */
#define KEY_STOP            DIK_STOP                /*                     (NEC PC98) */
#define KEY_AX              DIK_AX                  /*                     (Japan AX) */
#define KEY_UNLABELED       DIK_UNLABELED           /*                        (J3100) */
#define KEY_NEXTTRACK       DIK_NEXTTRACK           /* Next Track */
#define KEY_NUMPADENTER     DIK_NUMPADENTER         /* Enter on numeric keypad */
#define KEY_RCONTROL        DIK_RCONTROL
#define KEY_MUTE            DIK_MUTE                /* Mute */
#define KEY_CALCULATOR      DIK_CALCULATOR          /* Calculator */
#define KEY_PLAYPAUSE       DIK_PLAYPAUSE           /* Play / Pause */
#define KEY_MEDIASTOP       DIK_MEDIASTOP           /* Media Stop */
#define KEY_VOLUMEDOWN      DIK_VOLUMEDOWN          /* Volume - */
#define KEY_VOLUMEUP        DIK_VOLUMEUP            /* Volume + */
#define KEY_WEBHOME         DIK_WEBHOME             /* Web home */
#define KEY_NUMPADCOMMA     DIK_NUMPADCOMMA         /* , on numeric keypad (NEC PC98) */
#define KEY_DIVIDE          DIK_DIVIDE              /* / on numeric keypad */
#define KEY_SYSRQ           DIK_SYSRQ
#define KEY_RMENU           DIK_RMENU               /* right Alt */
#define KEY_PAUSE           DIK_PAUSE               /* Pause */
#define KEY_HOME            DIK_HOME                /* Home on arrow keypad */
#define KEY_UP              DIK_UP                  /* UpArrow on arrow keypad */
#define KEY_PRIOR           DIK_PRIOR               /* PgUp on arrow keypad */
#define KEY_LEFT            DIK_LEFT                /* LeftArrow on arrow keypad */
#define KEY_RIGHT           DIK_RIGHT               /* RightArrow on arrow keypad */
#define KEY_END             DIK_END                 /* End on arrow keypad */
#define KEY_DOWN            DIK_DOWN                /* DownArrow on arrow keypad */
#define KEY_NEXT            DIK_NEXT                /* PgDn on arrow keypad */
#define KEY_INSERT          DIK_INSERT              /* Insert on arrow keypad */
#define KEY_DELETE          DIK_DELETE              /* Delete on arrow keypad */
#define KEY_LWIN            DIK_LWIN                /* Left Windows key */
#define KEY_RWIN            DIK_RWIN                /* Right Windows key */
#define KEY_APPS            DIK_APPS                /* AppMenu key */
#define KEY_POWER           DIK_POWER               /* System Power */
#define KEY_SLEEP           DIK_SLEEP               /* System Sleep */
#define KEY_WAKE            DIK_WAKE                /* System Wake */
#define KEY_WEBSEARCH       DIK_WEBSEARCH           /* Web Search */
#define KEY_WEBFAVORITES    DIK_WEBFAVORITES        /* Web Favorites */
#define KEY_WEBREFRESH      DIK_WEBREFRESH          /* Web Refresh */
#define KEY_WEBSTOP         DIK_WEBSTOP             /* Web Stop */
#define KEY_WEBFORWARD      DIK_WEBFORWARD          /* Web Forward */
#define KEY_WEBBACK         DIK_WEBBACK             /* Web Back */
#define KEY_MYCOMPUTER      DIK_MYCOMPUTER          /* My Computer */
#define KEY_MAIL            DIK_MAIL                /* Mail */
#define KEY_MEDIASELECT     DIK_MEDIASELECT         /* Media Select */

#define KEY_BACKSPACE       KEY_BACK            /* backspace */
#define KEY_NUMPADSTAR      KEY_MULTIPLY        /* * on numeric keypad */
#define KEY_LALT            KEY_LMENU           /* left Alt */
#define KEY_CAPSLOCK        KEY_CAPITAL         /* CapsLock */
#define KEY_NUMPADMINUS     KEY_SUBTRACT        /* - on numeric keypad */
#define KEY_NUMPADPLUS      KEY_ADD             /* + on numeric keypad */
#define KEY_NUMPADPERIOD    KEY_DECIMAL         /* . on numeric keypad */
#define KEY_NUMPADSLASH     KEY_DIVIDE          /* / on numeric keypad */
#define KEY_RALT            KEY_RMENU           /* right Alt */
#define KEY_UPARROW         KEY_UP              /* UpArrow on arrow keypad */
#define KEY_PGUP            KEY_PRIOR           /* PgUp on arrow keypad */
#define KEY_LEFTARROW       KEY_LEFT            /* LeftArrow on arrow keypad */
#define KEY_RIGHTARROW      KEY_RIGHT           /* RightArrow on arrow keypad */
#define KEY_DOWNARROW       KEY_DOWN            /* DownArrow on arrow keypad */
#define KEY_PGDN            KEY_NEXT            /* PgDn on arrow keypad */

#define KEY_CIRCUMFLEX      KEY_PREVTRACK       /* Japanese keyboard */


//////////////////////////////////////////////////////////
// DIMOFS_ duplicates, again for dwEvent of INPUT_EVENT //
//////////////////////////////////////////////////////////
#define MOUSE_AXIS_X   DIMOFS_X  //for x, y, and z events
#define MOUSE_AXIS_Y   DIMOFS_Y
#define MOUSE_AXIS_Z   DIMOFS_Z
#define MOUSE_BUTT_0   DIMOFS_BUTTON0 //for button events
#define MOUSE_BUTT_1   DIMOFS_BUTTON1
#define MOUSE_BUTT_2   DIMOFS_BUTTON2
#define MOUSE_BUTT_3   DIMOFS_BUTTON3
#define MOUSE_BUTT_4   DIMOFS_BUTTON4
#define MOUSE_BUTT_5   DIMOFS_BUTTON5
#define MOUSE_BUTT_6   DIMOFS_BUTTON6
#define MOUSE_BUTT_7   DIMOFS_BUTTON7


#endif