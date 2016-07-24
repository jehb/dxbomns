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
// Graphics.cpp -- the graphics library class that's used by everything DX   //
// in DXBomns.  It's just a wrapper around the DDraw API, but if you wanted  //
// to port DXBomns to some OTHER API, it'd be a simple matter of rewriting   //
// this class.  Anyway, it's pretty simply laid out.                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include <stdio.h>
#include <string.h>

#include "DXBomns.h"
#include "XNotDirect.h"
#include "Graphics.h"

#pragma comment(lib, "ddraw")


extern HANDLE g_hHeap; //from DXBomns.cpp


BOOL InitGraphics(PGRAPHICS_VARS, HWND, UINT, UINT, UINT);          //various functions that aren't useful
VOID ReleaseGraphics(PGRAPHICS_VARS);                               //except in this module, so they're not
IDirectDrawSurface7 * CreateSurfaceFromFile(IDirectDraw7 *, PCSTR); //in the header (see their
BOOL ReloadSurfaceFromFile(IDirectDrawSurface7 *, PCSTR);           //definitions below)


//initializes a GRAPHICS_VARS structure (puts things into fullscreen mode et alii)
//the parameters are simple, for god's sake.
//returns whether it succeeded
BOOL InitGraphics(PGRAPHICS_VARS pgv, HWND hWnd, UINT unWidth, UINT unHeight, UINT unBPP)
{
	if(!pgv)
		return FALSE;
	
	//create the ddraw object
	if(DirectDrawCreateEx(NULL, (PVOID *)&pgv->pDD, IID_IDirectDraw7, NULL) != DD_OK)
	{
		Error("DirectDrawCreateEx", "unable to initialize DirectDraw 7.0a");
		return FALSE;
	}

	//set coop level
	if(pgv->pDD->SetCooperativeLevel(hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT) != DD_OK)
	{
		Error("IDirectDraw7::SetCooperativeLevel", "unable to initialize fullscreen control of the computer");
		return FALSE;
	}

	//set resolution
	if(pgv->pDD->SetDisplayMode(unWidth, unHeight, unBPP, 0, 0) != DD_OK)
	{
		Error("IDirectDraw7::SetDisplayMode", "unable to set the display mode... perhaps your monitor/video card doesn't support whatever mode we just tried?");
		return FALSE;
	}

	//create primary surface
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;
	if(pgv->pDD->CreateSurface(&ddsd, &pgv->pddsPrimary, NULL) != DD_OK)
	{
		Error("IDirectDraw7::CreateSurface", "unable to get a handle on the primary surface");
		return FALSE;
	}

	//and back surface
	DDSCAPS2 ddsc;
	ZeroMemory(&ddsc, sizeof(ddsc));
	ddsc.dwCaps = DDSCAPS_BACKBUFFER;
	if(pgv->pddsPrimary->GetAttachedSurface(&ddsc, &pgv->pddsBack) != DD_OK)
	{
		Error("IDirectDrawSurface7::GetAttachedSurface", "unable to get a handle on the back surface");
		return FALSE;
	}

	//create a clipper
	if(pgv->pDD->CreateClipper(0, &pgv->pddClipper, NULL) != DD_OK)
	{
		Error("IDirectDraw7::CreateClipper", "unable to create a clipper object");
		return FALSE;
	}

	//entire screen
	struct {RGNDATAHEADER h; RECT rc;} rd = {{sizeof(RGNDATAHEADER), RDH_RECTANGLES, 1, sizeof(RECT), {0, 0, unWidth, unHeight}}, {0, 0, unWidth, unHeight}};
	if(pgv->pddClipper->SetClipList((PRGNDATA)&rd, 0) != DD_OK)
	{
		Error("IDirectDrawClipper::SetClipList", "unable to set the clipper's clip list");
		return FALSE;
	}

	//attach it
	if(pgv->pddsBack->SetClipper(pgv->pddClipper) != DD_OK)
	{
		Error("IDirectDrawSurface7::SetClipper", "unable to give the clipper to the back buffer");
		return FALSE;
	}

	return TRUE;
}

//deallocates everything in InitializeGraphics
VOID ReleaseGraphics(PGRAPHICS_VARS pgv)
{
	if(pgv)
	{
		SAFERELEASE(pgv->pddClipper)
		SAFERELEASE(pgv->pddsBack)
		SAFERELEASE(pgv->pddsPrimary)
		SAFERELEASE(pgv->pDD)
	}
}

//creates, and returns, a new image surface from a bitmap file
//pDD is a pointer to an initialized DDraw object, cszFile is the bitmap file
//returns NULL if it fails
IDirectDrawSurface7 * CreateSurfaceFromFile(IDirectDraw7 * pDD, PCSTR cszFile)
{
	HBITMAP hImage = (HBITMAP)LoadImage(NULL, cszFile, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if(!hImage) //load the image
	{
		Error("LoadImage", "unable to load the specified image file");
		return NULL;
	}

	BITMAP bmpDesc;
	GetObject(hImage, sizeof(bmpDesc), &bmpDesc); //get its description

	DeleteObject(hImage);

	//create the surface
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = bmpDesc.bmWidth;
	ddsd.dwHeight = bmpDesc.bmHeight;
	IDirectDrawSurface7 * pRet = NULL;
	if(pDD->CreateSurface(&ddsd, &pRet, NULL) != DD_OK)
	{
		Error("IDirectDraw7::CreateSurface", "unable to create a surface from a file");
		return NULL;
	}

	//this is somewhat inefficient, as this just reopens the file and loads it in,
	//but it lets me just call ReloadSurface... when it gets reset, so it's ok
	if(!ReloadSurfaceFromFile(pRet, cszFile))
	{
		SAFERELEASE(pRet)
		return NULL;
	}

	return pRet;
}

//loads bitmap data, from the file specified by cszFile, into pddsOut
//returns whether it succeeded
BOOL ReloadSurfaceFromFile(IDirectDrawSurface7 * pddsOut, PCSTR cszFile)
{
	HBITMAP hImage = (HBITMAP)LoadImage(NULL, cszFile, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if(!hImage) //open the image
	{
		Error("LoadImage", "unable to load the specified image file");
		return FALSE;
	}

	BITMAP bmpDesc;
	GetObject(hImage, sizeof(bmpDesc), &bmpDesc); //get the description

	HDC hSurfaceDC = NULL;
	if(pddsOut->GetDC(&hSurfaceDC) != DD_OK) //get a DC
	{
		DeleteObject(hImage);
		Error("IDirectDrawSurface7::GetDC", "unable to get a DC from the surface");
		return FALSE;
	}

	HDC hImageDC = CreateCompatibleDC(hSurfaceDC);
	SelectObject(hImageDC, hImage);

	//blit the bitmap to the surface
	BitBlt(hSurfaceDC, 0, 0, bmpDesc.bmWidth, bmpDesc.bmHeight, hImageDC, 0, 0, SRCCOPY);

	DeleteDC(hImageDC);
	pddsOut->ReleaseDC(hSurfaceDC);
	DeleteObject(hImage);

	return TRUE;
}


//creates a graphics object
//hWnd is the current window, unWidth and unHeight and unBPP are the width
//and height and bits per pixel of the desired screen mode
//returns the created object, or NULL if failure
CGraphics * CreateGraphics(HWND hWnd, UINT unWidth, UINT unHeight, UINT unBPP)
{
	CGraphics * pRet = new CGraphics();
	if(!pRet)
	{
		Error("new CGraphics", "not enough memory");
		return NULL;
	}

	//initialize it
	if(!pRet->Initialize(hWnd, unWidth, unHeight, unBPP))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}


//just set everything to 0
CGraphics::CGraphics()
{
	ZeroMemory(&m_gvInnards, sizeof(m_gvInnards));
}

//release things
CGraphics::~CGraphics()
{
	ReleaseGraphics(&m_gvInnards);
}

//initializes the graphics for fullscreen
//simply a wrapper for the InitGraphics call with its innards
BOOL CGraphics::Initialize(HWND hWnd, UINT unWidth, UINT unHeight, UINT unBPP)
{
	return InitGraphics(&m_gvInnards, hWnd, unWidth, unHeight, unBPP);
}

//returns whether everything needs to be reset (if someone alt-tabbed)
BOOL CGraphics::NeedsReset()
{
	return (m_gvInnards.pddsPrimary->IsLost() == DDERR_SURFACELOST);
}

//resets all the actual ddraw surfaces (still call CImage::Reset, though)
//and clears the front buffer (why, I don't remember)
//returns whether it succeeded
BOOL CGraphics::Reset()
{
	if(m_gvInnards.pDD->RestoreAllSurfaces() != DD_OK) //restore all
	{
		Error("IDirectDraw7::RestoreAllSurfaces", "unable to restore all the surfaces");
		return FALSE;
	}

	DDBLTFX ddbfx;
	ZeroMemory(&ddbfx, sizeof(ddbfx)); //clear front buffer
	ddbfx.dwSize = sizeof(ddbfx);
	ddbfx.dwFillColor = 0;
	if(m_gvInnards.pddsPrimary->Blt(NULL, NULL, NULL, DDBLT_ASYNC | DDBLT_COLORFILL, &ddbfx) != DD_OK)
	{
		Error("IDirectDrawSurface7::Blt", "unable to clear the primary surface after resetting it");
		return FALSE;
	}

	return TRUE;
}

//easily the most important function in the file
//blits something (or many things) from one image to another
//pDest is the destination image (NULL for backbuffer)
//arcDest is an array of rectangles that define the destination spots on pDest
//        or, NULL for the entire surface
//unOps is the number of rectangles in that array (and arcSrc) (always specify at
//      least 1, even if arcDest and arcSrc are both NULL)
//pSrc is the source image (NULL for frontbuffer)
//arcSrc is an array of rectangles that define the source spots on pSrc
//       or, NULL for the entire surface
//returns whether it succeeded
//note that you CAN batch operations (specify unOps > 1, and the appropriate
//number of rectangles), though really, it won't speed anything up
BOOL CGraphics::Blit(CImage * pDest, PRECT arcDest, UINT unOps, CImage * pSrc, PRECT arcSrc)
{
	IDirectDrawSurface7 * pddsDest = (pDest ? pDest->GetSurface() : m_gvInnards.pddsBack); //get the appropriate ddraw surface
	if(!pddsDest)   //I can't remember now why this if statement is here, but I'll assume it's important
		return TRUE;
	IDirectDrawSurface7 * pddsSrc = (pSrc ? pSrc->GetSurface() : m_gvInnards.pddsPrimary); //get the appropriate ddraw surface

	for(UINT i = 0; i < unOps; i++) //for each rectangle
	{
		//blit it
		if(pddsDest->Blt((arcDest ? &arcDest[i] : NULL), pddsSrc, (arcSrc ? &arcSrc[i] : NULL), (pSrc && pSrc->Transparency() ? DDBLT_ASYNC | DDBLT_KEYSRC : DDBLT_ASYNC), NULL) != DD_OK)
		{
			Error("IDirectDrawSurface7::Blt", "unable to blit something");
			return FALSE;
		}
	}
	return TRUE;
}

//flips the back buffer to the front buffer
//returns whether it succeeded
BOOL CGraphics::Flip()
{
	if(m_gvInnards.pddsPrimary->Flip(NULL, 0) != DD_OK)
	{
		Error("IDirectDrawSurface7::Flip", "unable to flip the back buffer to the screen");
		return FALSE;
	}

	return TRUE;
}

//dumps a screenshot of the primary surface to a bitmap
//returns whether it succeeded
BOOL CGraphics::ScreenShot()
{
	CHAR szFile[32];
	for(INT i = 0; i < 1000; i++) //try all the sshotxxx.bmp bitmaps
	{
		sprintf(szFile, "sshot%03d.bmp", i);
		if(GetFileAttributes(szFile) == (DWORD)-1)
			break;
	}
	if(i == 1000) //if we couldn't find any unused names
	{
		Error("CGraphics::ScreenShot", "you already have 1000 screenshots!  Delete a few!");
		return FALSE;
	}

	//create the new file
	HANDLE hFile = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create a screenshot bitmap file");
		return FALSE;
	}

	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd)); //lock the primary surface
	ddsd.dwSize = sizeof(ddsd);
	if(m_gvInnards.pddsPrimary->Lock(NULL, &ddsd, DDLOCK_READONLY, NULL) != DD_OK)
	{
		CloseHandle(hFile);
		Error("IDirectDrawSurface7::Lock", "unable to lock the primary surface for a screenshot");
		return FALSE;
	}
	if(ddsd.ddpfPixelFormat.dwRGBBitCount != 16) //I didn't finish this code for anything but 16 bit mode
	{
		m_gvInnards.pddsPrimary->Unlock(NULL);
		CloseHandle(hFile);
		Error("CGraphics::ScreenShot", "taking screen shots while not in 16 bit mode isn't supported");
		return FALSE;
	}

	//create and write the file and info headers
	INT nBPP = ddsd.ddpfPixelFormat.dwRGBBitCount / 8;
	BITMAPFILEHEADER fh = {*(PWORD)"BM", sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ddsd.dwWidth * ddsd.dwHeight * nBPP, 0, 0, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)};
	BITMAPINFOHEADER ih = {sizeof(BITMAPINFOHEADER), ddsd.dwWidth, ddsd.dwHeight, 1, nBPP * 8, BI_RGB, 0, 0, 0, 0, 0};
	DWORD dwSize = sizeof(fh);
	if(!WriteFile(hFile, &fh, sizeof(fh), &dwSize, NULL) || dwSize != sizeof(fh))
	{
		m_gvInnards.pddsPrimary->Unlock(NULL);
		CloseHandle(hFile);
		Error("WriteFile", "unable to write the screen shot file header");
		return FALSE;
	}
	dwSize = sizeof(ih);
	if(!WriteFile(hFile, &ih, sizeof(ih), &dwSize, NULL) || dwSize != sizeof(ih))
	{
		m_gvInnards.pddsPrimary->Unlock(NULL);
		CloseHandle(hFile);
		Error("WriteFile", "unable to write the screen shot info header");
		return FALSE;
	}

	//a scan line
	PBYTE pyLine = new BYTE[ddsd.dwWidth * nBPP];
	if(!pyLine)
	{
		m_gvInnards.pddsPrimary->Unlock(NULL);
		CloseHandle(hFile);
		Error("new BYTE", "not enough memory");
		return FALSE;
	}
	for(i = 0; i < (INT)ddsd.dwHeight; i++) //for each line
	{
		//read the line from the surface
		CopyMemory(pyLine, &((PBYTE)ddsd.lpSurface)[(ddsd.dwHeight - i - 1) * ddsd.lPitch], ddsd.dwWidth * nBPP);
		for(INT j = 0; j < (INT)ddsd.dwWidth; j++) //for each pixel in the line
		{  //this for loop assumes that we're in 16 bit 565 mode
			INT nRed = (((PWORD)pyLine)[j] & 0xf800) >> 11; //translate it from 565 to 1555
			INT nGreen = (((PWORD)pyLine)[j] & 0x07e0) >> 6;//cut off an extra bit here
			INT nBlue = (((PWORD)pyLine)[j] & 0x001f) >> 0;

			((PWORD)pyLine)[j] = 0x7fff & ((nRed << 10) | (nGreen << 5) | (nBlue << 0));
		}

		dwSize = ddsd.dwWidth * nBPP; //write the line to the file
		if(!WriteFile(hFile, pyLine, ddsd.dwWidth * nBPP, &dwSize, NULL) || dwSize != ddsd.dwWidth * nBPP)
		{
			m_gvInnards.pddsPrimary->Unlock(NULL);
			SAFEDELETE(pyLine)
			CloseHandle(hFile);
			Error("WriteFile", "unable to write a screen shot scan line");
			return FALSE;
		}
	}
	SAFEDELETE(pyLine)

	CloseHandle(hFile);

	if(m_gvInnards.pddsPrimary->Unlock(NULL) != DD_OK) //unlock it
	{
		Error("IDirectDrawSurface7::Unlock", "unable to finish writing the primary surface to a screenshot");
		return FALSE;
	}

	return TRUE;
}

//creates, and returns, an image object, from a file or not
//cszFile is the bitmap file to use to create it, or NULL for none
//unWidth and unHeight are only used if cszFile is NULL
//bTrans is whether to create a color key for it
//and dwTrans is the transparent color, if bTrans is TRUE
//returns the object, or NULL if failure
CImage * CGraphics::CreateImage(PCSTR cszFile, UINT unWidth, UINT unHeight, BOOL bTrans, DWORD dwTrans)
{
	CImage * pRet = new CImage();
	if(!pRet)
	{
		Error("new CImage", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(m_gvInnards.pDD, cszFile, unWidth, unHeight, bTrans, dwTrans))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}


//zero everything
CImage::CImage()
{
	m_szFile = NULL;
	m_pddsImage = NULL;
	m_bColorKey = FALSE;
	m_pyBackup = NULL;
	m_bNeedsPaint = FALSE;
}

//delete everything we alloc'ed
CImage::~CImage()
{
	SAFEDELETEARRAY(m_szFile)
	SAFERELEASE(m_pddsImage)
	if(m_pyBackup)
		HeapFree(g_hHeap, 0, m_pyBackup);
}

//initializes the image, either from a file or not, with or without transparency.
//if cszFile is NULL, it uses unWidth and unHeight to create the image
//returns whether it succeeded
BOOL CImage::Initialize(IDirectDraw7 * pDD, PCSTR cszFile, UINT unWidth, UINT unHeight, BOOL bTrans, DWORD dwTrans)
{
	if(cszFile) //if they specified a file
	{
		m_szFile = new CHAR[strlen(cszFile) + 1]; //copy it here
		if(!m_szFile)
		{
			Error("new CHAR []", "not enough memory");
			return FALSE;
		}
		strcpy(m_szFile, cszFile);

		m_pddsImage = CreateSurfaceFromFile(pDD, m_szFile); //create the surface
		if(!m_pddsImage)
			return FALSE;

		m_bNeedsPaint = FALSE; //it's covered... don't paint it
	}
	else //they didn't specify a file
	{
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = unWidth;
		ddsd.dwHeight = unHeight;
		if(pDD->CreateSurface(&ddsd, &m_pddsImage, NULL) != DD_OK) //create a surface
		{
			Error("IDirectDraw7::CreateSurface", "unable to create an offscreen image");
			return FALSE;
		}

		if(!Clear(NULL, 0)) //clear it
			return FALSE;

		m_bNeedsPaint = TRUE; //it needs to be painted
	}

	if(bTrans) //if they indicated transparency
	{
		m_bColorKey = TRUE;
		DDCOLORKEY ddck = {dwTrans, dwTrans};
		if(m_pddsImage->SetColorKey(DDCKEY_SRCBLT, &ddck) != DD_OK) //set the color key
		{
			Error("IDirectDrawSurface7::SetColorKey", "unable to set transparent color");
			return FALSE;
		}
	}

	return TRUE;
}

//clears an image, or part of one, to a color
//if prcArea is NULL, it clears the entire image
//returns whether it succeeded
BOOL CImage::Clear(PRECT prcArea, DWORD dwColor)
{
	DDBLTFX ddbfx;
	ZeroMemory(&ddbfx, sizeof(ddbfx));
	ddbfx.dwSize = sizeof(ddbfx);
	ddbfx.dwFillColor = dwColor; //color fill blit
	if(m_pddsImage->Blt(prcArea, NULL, NULL, DDBLT_ASYNC | DDBLT_COLORFILL, &ddbfx) != DD_OK)
	{
		Error("IDirectDrawSurface7::Blt", "unable to clear a surface");
		return FALSE;
	}

	return TRUE;
}

//resets an image after someone alt-tabs
//automatically restores bitmaps, if it was created with a file
//otherwise, it just clears it and sets the paint flag
//returns whether it succeeded
BOOL CImage::Reset()
{
	//CGraphics::Reset() restores the actual DDraw surface, so we don't worry about that here

	if(m_szFile)
	{
		if(!ReloadSurfaceFromFile(m_pddsImage, m_szFile)) //reload it
			return FALSE;

		m_bNeedsPaint = FALSE;
	}
	else
	{
		if(!Clear(NULL, 0))  //clear it
			return FALSE;

		m_bNeedsPaint = TRUE;
	}

	return TRUE;
}

//simply returns whether this image needs to be painted
BOOL CImage::NeedsPainting() const
{
	return m_bNeedsPaint;
}

//sets or clears the paint flag
VOID CImage::SetPaintFlag(BOOL bNew)
{
	m_bNeedsPaint = bNew;
}

//creates a software memory backup of an image
//you may call Backup as many times as you like... the software memory buffer
//only retains the last call.  RestoreFromBackup doesn't actually deallocate
//the memory, so you can call THAT as many times as you like, too.
//this is useful for preserving images between alt-tabs... though don't call
//it right as the app goes inactive, because it'll fail (DDraw, at that point,
//has already lost control)
//returns whether it succeeded
BOOL CImage::Backup()
{
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	if(m_pddsImage->Lock(NULL, &ddsd, DDLOCK_READONLY, NULL) != DD_OK) //lock the surface
	{
		Error("IDirectDrawSurface7::Lock", "unable to backup an image");
		return FALSE;
	}

	INT nBPP = ddsd.ddpfPixelFormat.dwRGBBitCount / 8;
	if(m_pyBackup)
		HeapFree(g_hHeap, 0, m_pyBackup); //delete the previous backup, if any
	m_pyBackup = (PBYTE)HeapAlloc(g_hHeap, 0, ddsd.dwWidth * ddsd.dwHeight * nBPP); //alloc a new one
	if(!m_pyBackup)
	{
		m_pddsImage->Unlock(NULL);
		Error("HeapAlloc", "not enough memory");
		return FALSE;
	}

	for(INT i = 0; i < (INT)ddsd.dwHeight; i++) //copy that shit in
		CopyMemory(&m_pyBackup[i * ddsd.dwWidth * nBPP], &((PBYTE)ddsd.lpSurface)[i * ddsd.lPitch], ddsd.dwWidth * nBPP);

	if(m_pddsImage->Unlock(NULL) != DD_OK) //unlock it
	{
		Error("IDirectDrawSurface7::Unlock", "unable to finish backing up an image");
		return FALSE;
	}

	return TRUE;
}

//restores an image from the backup created by calling Backup().
//doesn't deallocate the memory, so it'll stay backed up through
//as many RestoreFromBackups you can call.
//returns whether it succeeded
BOOL CImage::RestoreFromBackup()
{
	if(m_pyBackup)
	{
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		if(m_pddsImage->Lock(NULL, &ddsd, DDLOCK_WRITEONLY, NULL) != DD_OK) //lock it
		{
			Error("IDirectDrawSurface7::Lock", "unable to restore an image");
			return FALSE;
		}

		INT nBPP = ddsd.ddpfPixelFormat.dwRGBBitCount / 8;
		for(INT i = 0; i < (INT)ddsd.dwHeight; i++)           //copy it in
			CopyMemory(&((PBYTE)ddsd.lpSurface)[i * ddsd.lPitch], &m_pyBackup[i * ddsd.dwWidth * nBPP], ddsd.dwWidth * nBPP);

		if(m_pddsImage->Unlock(NULL) != DD_OK) //unlock it
		{
			Error("IDirectDrawSurface7::Unlock", "unable to finish restoring up an image");
			return FALSE;
		}

		m_bNeedsPaint = FALSE; //it's good, don't paint it
	}
	return TRUE;
}

//accessor for Blit... simply gets the DDraw surface
IDirectDrawSurface7 * CImage::GetSurface() const
{
	return m_pddsImage;
}

//another accessor for Blit... returns whether it's got a color key
BOOL CImage::Transparency() const
{
	return m_bColorKey;
}