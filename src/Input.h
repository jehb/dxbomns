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
// Input.h -- the header for the game's input system.  Some day this         //
// might become a library all its own.  Basically, it's just a DX wrapper.   //
// Anyway, look at the .cpp file for more info.                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __INPUT_H__
#define __INPUT_H__


class CInput;
class CDevice;


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "InputEx.h"


#define DEVICE_BUFFERSIZE 32 //how many elements in the device's buffer


typedef enum _DEVICE_TYPE //what type of device is it?
{
	DT_NONE = 0,  //will fail
	DT_KEYBOARD,  //keyboard
	DT_MOUSE,     //mouse
	DT_JOY,       //unsupported
	DT_OTHER,     //unsupported

	DT_FORCE_DWORD = 0x7fffffff //don't use
} DEVICE_TYPE, * PDEVICE_TYPE;


typedef struct _INPUT_VARS //the internal input system variables
{
	IDirectInput8 * pDI;  //direct input
	HWND            hWnd; //the window
} INPUT_VARS, * PINPUT_VARS;


class CInput //the input system class
{
	public:
		CInput();
		~CInput();

		BOOL Initialize(HINSTANCE, HWND); //initialize it

		CDevice * CreateDevice(DEVICE_TYPE, BOOL (*)(PINPUT_EVENT)); //create a device

	private:
		INPUT_VARS m_ivInnards; //internal dinput vars

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CInput(const CInput &);
		VOID operator = (const CInput &);
};

class CDevice //the device class
{
	public:
		CDevice();
		~CDevice();

		BOOL Initialize(IDirectInput8 *, HWND, DEVICE_TYPE, BOOL (*)(PINPUT_EVENT)); //initialize

		BOOL Update(); //each frame

	private:
		IDirectInputDevice8 * m_pdiDev; //the actual device
		DEVICE_TYPE           m_dtType; //what type it is
		BOOL               (* m_pfnOnInput)(PINPUT_EVENT); //what function to call on getting input

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CDevice(const CDevice &);
		VOID operator = (const CDevice &);
};


CInput * CreateInput(HINSTANCE, HWND); //create an input system
BYTE StringToKey(PCSTR, BYTE);         //turn a string into a KEY_ constant


#endif