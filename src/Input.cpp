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
// Input.cpp -- the input library class that's used by everything DX in      //
// DXBomns.  It's just a wrapper around the DInput API, but if you wanted    //
// to port DXBomns to some OTHER API, it'd be a simple matter of rewriting   //
// this class.  Anyway, it's pretty simply laid out.                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include "DXBomns.h"
#include "XNotDirect.h"
#include "Input.h"

#pragma comment(lib, "dinput8")


BOOL InitInput(PINPUT_VARS, HINSTANCE, HWND); //these functions are only useful in this module
VOID ReleaseInput(PINPUT_VARS);               //see their definitions below


//initializes DInput into a dinput vars struct
//returns whether it succeeded
BOOL InitInput(PINPUT_VARS piv, HINSTANCE hInst, HWND hWnd)
{
	if(!piv)
		return FALSE;

	piv->hWnd = hWnd; //copy the window, create the dinput object
	if(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (PVOID *)&piv->pDI, NULL) != DI_OK)
	{
		Error("DirectInput8Create", "unable to initialize DirectInput8");
		return FALSE;
	}

	return TRUE;
}

//deallocates anything that was allocated in InitInput
VOID ReleaseInput(PINPUT_VARS piv)
{
	if(piv)
	{
		piv->hWnd = NULL;
		SAFERELEASE(piv->pDI)
	}
}


//creates a CInput object
//returns NULL if it fails
CInput * CreateInput(HINSTANCE hInst, HWND hWnd)
{
	CInput * pRet = new CInput();
	if(!pRet)
	{
		Error("new CInput", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(hInst, hWnd))
	{
		SAFEDELETE(pRet);
		return NULL;
	}

	return pRet;
}

//turns a string into a KEY_ constant
//if it can't find a match, it returns yDefault
BYTE StringToKey(PCSTR cszStr, BYTE yDefault)
{
	if(!_stricmp(cszStr, "a"))           return KEY_A;
	if(!_stricmp(cszStr, "b"))           return KEY_B;
	if(!_stricmp(cszStr, "c"))           return KEY_C;
	if(!_stricmp(cszStr, "d"))           return KEY_D;
	if(!_stricmp(cszStr, "e"))           return KEY_E;
	if(!_stricmp(cszStr, "f"))           return KEY_F;
	if(!_stricmp(cszStr, "g"))           return KEY_G;
	if(!_stricmp(cszStr, "h"))           return KEY_H;
	if(!_stricmp(cszStr, "i"))           return KEY_I;
	if(!_stricmp(cszStr, "j"))           return KEY_J;
	if(!_stricmp(cszStr, "k"))           return KEY_K;
	if(!_stricmp(cszStr, "l"))           return KEY_L;
	if(!_stricmp(cszStr, "m"))           return KEY_M;
	if(!_stricmp(cszStr, "n"))           return KEY_N;
	if(!_stricmp(cszStr, "o"))           return KEY_O;
	if(!_stricmp(cszStr, "p"))           return KEY_P;
	if(!_stricmp(cszStr, "q"))           return KEY_Q;
	if(!_stricmp(cszStr, "r"))           return KEY_R;
	if(!_stricmp(cszStr, "s"))           return KEY_S;
	if(!_stricmp(cszStr, "t"))           return KEY_T;
	if(!_stricmp(cszStr, "u"))           return KEY_U;
	if(!_stricmp(cszStr, "v"))           return KEY_V;
	if(!_stricmp(cszStr, "w"))           return KEY_W;
	if(!_stricmp(cszStr, "x"))           return KEY_X;
	if(!_stricmp(cszStr, "y"))           return KEY_Y;
	if(!_stricmp(cszStr, "z"))           return KEY_Z;
	if(!_stricmp(cszStr, "1"))           return KEY_1;
	if(!_stricmp(cszStr, "2"))           return KEY_2;
	if(!_stricmp(cszStr, "3"))           return KEY_3;
	if(!_stricmp(cszStr, "4"))           return KEY_4;
	if(!_stricmp(cszStr, "5"))           return KEY_5;
	if(!_stricmp(cszStr, "6"))           return KEY_6;
	if(!_stricmp(cszStr, "7"))           return KEY_7;
	if(!_stricmp(cszStr, "8"))           return KEY_8;
	if(!_stricmp(cszStr, "9"))           return KEY_9;
	if(!_stricmp(cszStr, "0"))           return KEY_0;
	if(!_stricmp(cszStr, "-"))           return KEY_MINUS;
	if(!_stricmp(cszStr, "="))           return KEY_EQUALS;
	if(!_stricmp(cszStr, "backspace"))   return KEY_BACKSPACE;
	if(!_stricmp(cszStr, "tab"))         return KEY_TAB;
	if(!_stricmp(cszStr, "["))           return KEY_LBRACKET;
	if(!_stricmp(cszStr, "]"))           return KEY_RBRACKET;
	if(!_stricmp(cszStr, "enter"))       return KEY_RETURN;
	if(!_stricmp(cszStr, "lcontrol"))    return KEY_LCONTROL;
	if(!_stricmp(cszStr, "rcontrol"))    return KEY_RCONTROL;
	if(!_stricmp(cszStr, ";"))           return KEY_SEMICOLON;
	if(!_stricmp(cszStr, "'"))           return KEY_APOSTROPHE;
	if(!_stricmp(cszStr, "`"))           return KEY_GRAVE;
	if(!_stricmp(cszStr, "lshift"))      return KEY_LSHIFT;
	if(!_stricmp(cszStr, "rshift"))      return KEY_RSHIFT;
	if(!_stricmp(cszStr, "\\"))          return KEY_BACKSLASH;
	if(!_stricmp(cszStr, ","))           return KEY_COMMA;
	if(!_stricmp(cszStr, "."))           return KEY_PERIOD;
	if(!_stricmp(cszStr, "/"))           return KEY_SLASH;
	if(!_stricmp(cszStr, "lalt"))        return KEY_LALT;
	if(!_stricmp(cszStr, "ralt"))        return KEY_RALT;
	if(!_stricmp(cszStr, "space"))       return KEY_SPACE;
	if(!_stricmp(cszStr, "capslock"))    return KEY_CAPSLOCK;
	if(!_stricmp(cszStr, "f1"))          return KEY_F1;
	if(!_stricmp(cszStr, "f2"))          return KEY_F2;
	if(!_stricmp(cszStr, "f3"))			 return KEY_F3;
	if(!_stricmp(cszStr, "f4"))          return KEY_F4;
	if(!_stricmp(cszStr, "f5"))          return KEY_F5;
	if(!_stricmp(cszStr, "f6"))          return KEY_F6;
	if(!_stricmp(cszStr, "f7"))          return KEY_F7;
	if(!_stricmp(cszStr, "f8"))          return KEY_F8;
	if(!_stricmp(cszStr, "f9"))          return KEY_F9;
	if(!_stricmp(cszStr, "f10"))         return KEY_F10;
	if(!_stricmp(cszStr, "f11"))         return KEY_F11;
	if(!_stricmp(cszStr, "f12"))         return KEY_F12;
	if(!_stricmp(cszStr, "f12"))         return KEY_F12;
	if(!_stricmp(cszStr, "lwin"))        return KEY_LWIN;
	if(!_stricmp(cszStr, "rwin"))        return KEY_RWIN;
	if(!_stricmp(cszStr, "appmenu"))     return KEY_APPS;
	if(!_stricmp(cszStr, "left"))        return KEY_LEFT;
	if(!_stricmp(cszStr, "right"))       return KEY_RIGHT;
	if(!_stricmp(cszStr, "up"))          return KEY_UP;
	if(!_stricmp(cszStr, "down"))        return KEY_DOWN;
	if(!_stricmp(cszStr, "insert"))      return KEY_INSERT;
	if(!_stricmp(cszStr, "delete"))      return KEY_DELETE;
	if(!_stricmp(cszStr, "home"))        return KEY_HOME;
	if(!_stricmp(cszStr, "end"))         return KEY_END;
	if(!_stricmp(cszStr, "pageup"))      return KEY_PGUP;
	if(!_stricmp(cszStr, "pagedown"))    return KEY_PGDN;
	if(!_stricmp(cszStr, "numlock"))     return KEY_NUMLOCK;
	if(!_stricmp(cszStr, "numpad/"))     return KEY_DIVIDE;
	if(!_stricmp(cszStr, "numpad*"))     return KEY_MULTIPLY;
	if(!_stricmp(cszStr, "numpad-"))     return KEY_SUBTRACT;
	if(!_stricmp(cszStr, "numpad+"))     return KEY_ADD;
	if(!_stricmp(cszStr, "numpad1"))     return KEY_NUMPAD1;
	if(!_stricmp(cszStr, "numpad2"))     return KEY_NUMPAD2;
	if(!_stricmp(cszStr, "numpad3"))     return KEY_NUMPAD3;
	if(!_stricmp(cszStr, "numpad4"))     return KEY_NUMPAD4;
	if(!_stricmp(cszStr, "numpad5"))     return KEY_NUMPAD5;
	if(!_stricmp(cszStr, "numpad6"))     return KEY_NUMPAD6;
	if(!_stricmp(cszStr, "numpad7"))     return KEY_NUMPAD7;
	if(!_stricmp(cszStr, "numpad8"))     return KEY_NUMPAD8;
	if(!_stricmp(cszStr, "numpad9"))     return KEY_NUMPAD9;
	if(!_stricmp(cszStr, "numpad0"))     return KEY_NUMPAD0;
	if(!_stricmp(cszStr, "numpad."))     return KEY_DECIMAL;
	if(!_stricmp(cszStr, "numpadenter")) return KEY_NUMPADENTER;

	return yDefault;
}


//zero everything
CInput::CInput()
{
	ZeroMemory(&m_ivInnards, sizeof(m_ivInnards));
}

//release input
CInput::~CInput()
{
	ReleaseInput(&m_ivInnards);
}

//initializes input
//returns whether it succeeded
//(just a wrapper for InitInput, that uses this object's innards)
BOOL CInput::Initialize(HINSTANCE hInst, HWND hWnd)
{
	return InitInput(&m_ivInnards, hInst, hWnd);
}

//creates a device.. DT_KEYBOARD and DT_MOUSE are the only supported types
//pfnOnInput is a pointer to a function that takes a pointer to an INPUT_EVENT struct
//that defines the input event as a parameter, and returns whether it encountered
//success in processing the message (in other words, if IT returns false, so does
//the input processor function).  DON'T deallocate the class from this function,
//as it'll crash.  Set a flag and deallocate it later or something
//returns NULL if it fails to create the device
CDevice * CInput::CreateDevice(DEVICE_TYPE dt, BOOL (* pfnOnInput)(PINPUT_EVENT))
{
	CDevice * pRet = new CDevice();
	if(!pRet)
	{
		Error("new CDevice", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(m_ivInnards.pDI, m_ivInnards.hWnd, dt, pfnOnInput))
	{
		SAFEDELETE(pRet);
		return NULL;
	}

	return pRet;
}


//zero things
CDevice::CDevice()
{
	m_pdiDev = NULL;
	m_dtType = DT_NONE;
	m_pfnOnInput = NULL;
}

//deallocate
CDevice::~CDevice()
{
	if(m_pdiDev)
		m_pdiDev->Unacquire();
	SAFERELEASE(m_pdiDev)
}

//initializes the device.  See CreateDevice for info on pfnOnInput
//returns whether it was successful
BOOL CDevice::Initialize(IDirectInput8 * pDI, HWND hWnd, DEVICE_TYPE dt, BOOL (* pfnOnInput)(PINPUT_EVENT))
{
	m_dtType = dt;
	m_pfnOnInput = pfnOnInput;

	//create the device object
	if(pDI->CreateDevice((dt == DT_KEYBOARD ? GUID_SysKeyboard : (dt == DT_MOUSE ? GUID_SysMouse : GUID_NULL)), &m_pdiDev, NULL) != DI_OK)
	{
		Error("IDirectInput8::CreateDevice", "unable to create an input device");
		return FALSE;
	}

	//set the format
	if(m_pdiDev->SetDataFormat(dt == DT_KEYBOARD ? &c_dfDIKeyboard : (dt == DT_MOUSE ? &c_dfDIMouse : NULL)) != DI_OK)
	{
		Error("IDirectInputDevice8::SetDataFormat", "unable to set the default data format for an input device");
		return FALSE;
	}

	//set coop level
	if(m_pdiDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE) != DI_OK)
	{
		Error("IDirectInputDevice8::SetCooperativeLevel", "unable to set the coop level for an input device");
		return FALSE;
	}

	//set it up for buffered input
	DIPROPDWORD pd = {{sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), 0, DIPH_DEVICE}, DEVICE_BUFFERSIZE};
	if(m_pdiDev->SetProperty(DIPROP_BUFFERSIZE, &pd.diph) != DI_OK)
	{
		Error("IDirectInputDevice8::SetProperty", "unable to set the buffer size for an input device");
		return FALSE;
	}
	
	//and acquire it
	if(m_pdiDev->Acquire() != DI_OK)
	{
		Error("IDirectInputDevice8::Acquire", "unable to acquire the input device");
		return FALSE;
	}

	return TRUE;
}

//call each frame
//returns whether it succeeded
BOOL CDevice::Update()
{
	DWORD dwItems = 0;

	//check for being lost
	if(m_pdiDev->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &dwItems, DIGDD_PEEK) == DIERR_INPUTLOST)
	{
		if(m_pdiDev->Acquire() != DI_OK) //if it can't reacquire it
		{
			Error("IDirectInputDevice8::Acquire", "unable to reacquire the input device");
			return FALSE;
		}
	}

	//get the data
	DIDEVICEOBJECTDATA adodData[DEVICE_BUFFERSIZE];
	dwItems = DEVICE_BUFFERSIZE;
	if(m_pdiDev->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), adodData, &dwItems, 0) != DI_OK)
	{
		Error("IDirectInputDevice8::GetDeviceData", "unable to read data from an input device");
		return FALSE;
	}

	for(UINT i = 0; i < dwItems; i++) //call pfnOnInput for each
	{
		INPUT_EVENT ie = {adodData[i].dwOfs, (LONG)adodData[i].dwData, adodData[i].dwTimeStamp};
		if(m_dtType == DT_KEYBOARD || (m_dtType == DT_MOUSE && ie.dwEvent != MOUSE_AXIS_X && ie.dwEvent != MOUSE_AXIS_Y && ie.dwEvent != MOUSE_AXIS_Z))
		{
			if(ie.lStatus & 0x80)//if it's a button style event
				ie.lStatus = TRUE;//forget this bit shit, just turn it into a bool
			else
				ie.lStatus = FALSE;
		}

		if(!m_pfnOnInput(&ie))//call the function
			return FALSE;
	}

	return TRUE;
}