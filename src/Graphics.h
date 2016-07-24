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
// Graphics.h -- the header for the game's graphics system.  Some day this   //
// might become a library all its own.  Basically, it's just a DX wrapper.   //
// Anyway, look at the .cpp file for more info.                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__


class CGraphics;
class CImage;


#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>


typedef struct _GRAPHICS_VARS //the variables necessary for dx graphics stuff
{
	IDirectDraw7        * pDD;         //the ddraw object
	IDirectDrawSurface7 * pddsPrimary; //primary
	IDirectDrawSurface7 * pddsBack;    //and back surfaces
	IDirectDrawClipper  * pddClipper;  //and a clipper to clip back buffer
} GRAPHICS_VARS, * PGRAPHICS_VARS;


class CGraphics //the graphics system class
{
	public:
		CGraphics();
		~CGraphics();

		BOOL Initialize(HWND, UINT, UINT, UINT); //initialize it

		BOOL NeedsReset(); //whether it needs to be reset
		BOOL Reset();      //reset it

		BOOL Blit(CImage *, PRECT, UINT, CImage *, PRECT); //blit something
		BOOL Flip(); //back -> front

		BOOL ScreenShot(); //dump screen to bitmap

		CImage * CreateImage(PCSTR, UINT, UINT, BOOL, DWORD); //create an image

	private:
		GRAPHICS_VARS m_gvInnards; //the internal variables

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CGraphics(const CGraphics &);
		VOID operator = (const CGraphics &);
};

class CImage //the image class
{
	public:
		CImage();
		~CImage();

		BOOL Initialize(IDirectDraw7 *, PCSTR, UINT, UINT, BOOL, DWORD); //initialize it

		BOOL Clear(PRECT, DWORD); //clear it to a color
		
		BOOL Reset(); //reset it
		BOOL NeedsPainting() const; //does it need to be repainted?
		VOID SetPaintFlag(BOOL);    //set/clear paint flag
		BOOL Backup();              //back it up to system memory
		BOOL RestoreFromBackup();   //restore it from system memory

		IDirectDrawSurface7 * GetSurface() const; //accessor for CGraphics
		BOOL Transparency() const;                //whether it's got a colorkey defined

	private:
		PSTR                  m_szFile;      //the image file it's associated with (NULL for none)
		IDirectDrawSurface7 * m_pddsImage;   //the surface
		BOOL                  m_bColorKey;   //whether it uses transparency

		PBYTE                 m_pyBackup;    //the backup system memory (NULL if not backed up)
		
		BOOL                  m_bNeedsPaint; //whether it needs to be painted

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CImage(const CImage &);
		VOID operator = (const CImage &);
};


CGraphics * CreateGraphics(HWND, UINT, UINT, UINT); //create a graphics system


#endif