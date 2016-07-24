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
// XNotDirect.h -- header for various ALMOST dx-specific things, but really, //
// just controls things like always setting up fullscreen and everything     //
// correctly.  Look at the .cpp for more info.                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __XNOTDIRECT_H__
#define __XNOTDIRECT_H__


#include "InputEx.h"


#define SCR_WIDTH 1024 //screen mode
#define SCR_HEIGHT 768
#define SCR_BPP     16


#define SAFERELEASE(x) {if(x) {(x)->Release(); (x) = NULL;}} //release a COM object


BOOL InitDX(BOOL (*)(PINPUT_EVENT), BOOL (*)(PINPUT_EVENT)); //init DX for fullscreen deals
VOID CloseDX(); //close it


#endif