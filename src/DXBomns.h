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
// DXBomns.h -- the DXBomns main header file, included by just about         //
// everything.  See the .cpp for more info.                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __DXBOMNS_H__
#define __DXBOMNS_H__


#include "Ver.h"


#define SAFEDELETE(x)      {if(x) {delete    (x); (x) = NULL;}} //safely deletes something
#define SAFEDELETEARRAY(a) {if(a) {delete [] (a); (a) = NULL;}} //safely deletes an array


INT WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, INT); //WinMain, of course
BOOL EnterStartupDialog();                           //go to startup dialog
INT_PTR CALLBACK StartupDialogProc(HWND, UINT, WPARAM, LPARAM); //startup proc
VOID __logerror(PCSTR, UINT, PCSTR, PCSTR);                     //logs an error
#define Error(f, m) __logerror(__FILE__, __LINE__, f, m)        //macro to automatically include file and line in the error log


#endif