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
// Font.h -- definitions and such for the ingame font.  See the .cpp file    //
// for more info.                                                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __FONT_H__
#define __FONT_H__


class CFont;


#include "Graphics.h"


class CFont //the font class
{
	public:
		CFont();
		~CFont();

		BOOL Initialize(CGraphics *); //initialize

		BOOL Reset(); //reset all the images

		BOOL Box(CImage *, INT, INT, INT, INT); //draw a box
		BOOL Scribe(CImage *, INT, INT, PCSTR, ...); //draw some text
		BOOL Draw(CImage *, PCSTR, ...);//box + scribe

	private:
		BOOL RawWrite(CImage *, INT, INT, PCSTR); //don't use vargs stuff
		VOID GetDimensions(PCSTR, PINT, PINT);    //get the dimensions of a string of text

		CGraphics * m_pCopyGraphics; //copy of the game's graphics system

		CImage    * m_piBox; //the box image
		CImage    * m_piFont;//font image

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CFont(const CFont &);
		VOID operator = (const CFont &);
};


CFont * CreateFont(CGraphics *); //create a font object


#endif