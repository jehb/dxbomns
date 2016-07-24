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
// EditorDialog.h -- prototypes for the dxbomns editor dialog.  See the .cpp //
// for more info.                                                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __EDITORDIALOG_H__
#define __EDITORDIALOG_H__


BOOL EnterEditorDialog(); //go to dialog
INT_PTR CALLBACK EditorDialogProc(HWND, UINT, WPARAM, LPARAM); //proc for main dlg
INT_PTR CALLBACK GenDialogProc(HWND, UINT, WPARAM, LPARAM);    //proc for generate dlg
INT_PTR CALLBACK KeysDialogProc(HWND, UINT, WPARAM, LPARAM);   //for keys dlg


#endif