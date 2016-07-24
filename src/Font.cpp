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
// Font.cpp -- the in-game text-drawing class.  Also draws boxes behind text //
// if that's your thing.                                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include <stdio.h>
#include <stdarg.h>

#include "DXBomns.h"
#include "XNotDirect.h"
#include "Graphics.h"
#include "Font.h"


//creates a font object and initializes it
//returns a pointer to the object on success, NULL on fail
//requires a valid, initialized CGraphics object
CFont * CreateFont(CGraphics * pGraphics)
{
	CFont * pRet = new CFont();
	if(!pRet)
	{
		Error("new CFont", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(pGraphics))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}


//just sets things to NULL
CFont::CFont()
{
	m_pCopyGraphics = NULL;
	m_piBox = NULL;
	m_piFont = NULL;
}

//and deletes whatever's not null
CFont::~CFont()
{
	SAFEDELETE(m_piFont)
	SAFEDELETE(m_piBox)
	//don't delete CopyGraphics, as it's a copy
}

//initializes the object... should only be called by CreateFont
//requires a valid CGraphics object, returns whether it succeeded
BOOL CFont::Initialize(CGraphics * pGraphics)
{
	m_pCopyGraphics = pGraphics;
	
	m_piBox = m_pCopyGraphics->CreateImage(".\\gfx\\box.bmp", 0, 0, TRUE, 0xf81f); //bright pink is transparent
	if(!m_piBox) //create the box image
		return FALSE;

	m_piFont = m_pCopyGraphics->CreateImage(".\\gfx\\font.bmp", 0, 0, TRUE, 0x0000); //black is transparent
	if(!m_piFont) //and the font image
		return FALSE;

	return TRUE;
}

//resets the images if someone alt-tabs
//returns whether successful
BOOL CFont::Reset()
{
	if(!m_piBox->Reset())
		return FALSE;
	if(!m_piFont->Reset())
		return FALSE;

	return TRUE;
}

//draws a box
//takes: the destination image (NULL for back buffer),
//left and top are PIXELS, not game tiles
//and how many characters wide and tall to make the box
//returns whether it succeeded
BOOL CFont::Box(CImage * piDest, INT nLeft, INT nTop, INT nCharsWide, INT nCharsTall)
{
	for(INT i = 0; i < nCharsTall; i++)
	{
		for(INT j = 0; j < nCharsWide; j++)
		{
			RECT rcDest;
			rcDest.left   = nLeft + (j << 4); //set up the dest rect
			rcDest.top    = nTop + (i << 4);
			rcDest.right  = rcDest.left + 16;
			rcDest.bottom = rcDest.top + 16;
			RECT rcSource;
			rcSource.left   = (j == 0 ? 0 : (j == nCharsWide - 1 ? 2 : 1)) << 4; //and source rect
			rcSource.top    = (i == 0 ? 0 : (i == nCharsTall - 1 ? 2 : 1)) << 4;
			rcSource.right  = rcSource.left + 16;
			rcSource.bottom = rcSource.top + 16;

			if(!m_pCopyGraphics->Blit(piDest, &rcDest, 1, m_piBox, &rcSource)) //blit it
				return FALSE;
		}
	}

	return TRUE;
}

//draws some text (no \ characters except for \n supported)
//piDest is the destination image (NULL for backbuffer)
//left, top are in pixels, not game tiles
//and cszFormat is the string... you may use printf specifiers in it,
//and pass the args in the ... parameter
//returns whether it succeeded
BOOL CFont::Scribe(CImage * piDest, INT nLeft, INT nTop, PCSTR cszFormat, ...)
{
	va_list vList;
	va_start(vList, cszFormat); //get the full string from the format string
	CHAR szOut[2048];
	vsprintf(szOut, cszFormat, vList);
	va_end(vList);

	return RawWrite(piDest, nLeft, nTop, szOut); //see below
}

//draw a box behind some text, centered in the screen
//simply takes destination (NULL if back buffer), and the string (use printf args)
//returns whether it succeeded
BOOL CFont::Draw(CImage * piDest, PCSTR cszFormat, ...)
{
	va_list vList;
	va_start(vList, cszFormat);
	CHAR szOut[2048];
	vsprintf(szOut, cszFormat, vList); //set up output string from cszFormat
	va_end(vList);

	INT nCharsWide = 0, nCharsTall = 0;
	GetDimensions(szOut, &nCharsWide, &nCharsTall); //get the string's dimensions (see below)

	INT nLeft = (SCR_WIDTH - (nCharsWide << 4)) >> 1; //center
	INT nTop = (SCR_HEIGHT - (nCharsTall << 4)) >> 1;

	if(!Box(piDest, nLeft - 16, nTop - 16, nCharsWide + 2, nCharsTall + 2)) //draw a box
		return FALSE;
	
	if(!RawWrite(piDest, nLeft, nTop, szOut)) //and the text over it
		return FALSE;

	return TRUE;
}

//writes a string withOUT printf args
//called by Draw and Scribe (so see them, too)
BOOL CFont::RawWrite(CImage * piDest, INT nLeft, INT nTop, PCSTR cszStr)
{
	if(!cszStr)
		return TRUE;

	INT nX = 0, nY = 0;
	while(*cszStr)
	{
		if(*cszStr == '\n') //go to next line
		{
			nX = 0;
			nY++;
			cszStr++;
			continue;
		}

		INT nCharX;
		switch(*cszStr) //get the char's x position (in 16 unit tiles)
		{
			case 'a': case 'A': case 'o': case 'O': case '2': case '^' : case ';' : nCharX =  0; break;
			case 'b': case 'B': case 'p': case 'P': case '3': case '&' : case '<' : nCharX =  1; break;
			case 'c': case 'C': case 'q': case 'Q': case '4': case '*' : case '>' : nCharX =  2; break;
			case 'd': case 'D': case 'r': case 'R': case '5': case '(' : case '/' : nCharX =  3; break;
			case 'e': case 'E': case 's': case 'S': case '6': case ')' : case '\\': nCharX =  4; break;
			case 'f': case 'F': case 't': case 'T': case '7': case '-' : case '[' : nCharX =  5; break;
			case 'g': case 'G': case 'u': case 'U': case '8': case '_' : case ']' : nCharX =  6; break;
			case 'h': case 'H': case 'v': case 'V': case '9': case '+' : case '{' : nCharX =  7; break;
			case 'i': case 'I': case 'w': case 'W': case '~': case '=' : case '}' : nCharX =  8; break;
			case 'j': case 'J': case 'x': case 'X': case '!': case '.' : case '?' : nCharX =  9; break;
			case 'k': case 'K': case 'y': case 'Y': case '@': case ',' : case '|' : nCharX = 10; break;
			case 'l': case 'L': case 'z': case 'Z': case '#': case '"' :            nCharX = 11; break;
			case 'm': case 'M': case '0':           case '$': case '\'':            nCharX = 12; break;
			default :                                                               nCharX = 13; break;
		}

		INT nCharY;
		switch(*cszStr) //and its y position (same 16 unit tiles)
		{
			case 'a': case 'A': case 'b': case 'B': case 'c' : case 'C': case 'd': case 'D':
			case 'e': case 'E': case 'f': case 'F': case 'g' : case 'G': case 'h': case 'H':
			case 'i': case 'I': case 'j': case 'J': case 'k' : case 'K': case 'l': case 'L':
			case 'm': case 'M': case 'n': case 'N': nCharY = 0; break;
			case 'o': case 'O': case 'p': case 'P': case 'q' : case 'Q': case 'r': case 'R':
			case 's': case 'S': case 't': case 'T': case 'u' : case 'U': case 'v': case 'V':
			case 'w': case 'W': case 'x': case 'X': case 'y' : case 'Y': case 'z': case 'Z':
			case '0': case '1': nCharY = 1; break;
			case '2': case '3': case '4': case '5': case '6' : case '7': case '8': case '9':
			case '~': case '!': case '@': case '#': case '$' : case '%': nCharY = 2; break;
			case '^': case '&': case '*': case '(': case ')' : case '-': case '_': case '+':
			case '=': case '.': case ',': case '"': case '\'': case ':': nCharY = 3; break;
			default : nCharY = 4; break;
		}

		RECT rcDest;
		rcDest.left   = nLeft + (nX << 4); //set up the dest rect
		rcDest.top    = nTop + (nY << 4);
		rcDest.right  = rcDest.left + 16;
		rcDest.bottom = rcDest.top + 16;
		RECT rcSource;
		rcSource.left   = nCharX << 4; //and source rect
		rcSource.top    = nCharY << 4;
		rcSource.right  = rcSource.left + 16;
		rcSource.bottom = rcSource.top + 16;
		if(!m_pCopyGraphics->Blit(piDest, &rcDest, 1, m_piFont, &rcSource)) //blit it
			return FALSE;

		nX++;
		cszStr++; //increment
	}

	return TRUE;
}

//gets the dimensions of a string
//pnCharsWide and pnCharsTall [both out] are how wide and tall the string is on return
VOID CFont::GetDimensions(PCSTR cszStr, PINT pnCharsWide, PINT pnCharsTall)
{
	if(!cszStr)
		return;

	INT nLongest = 0; //the longest (width) string we've found yet
	INT nWidth = 0;   //the current width
	INT nHeight = 1;  //total height

	while(*cszStr)
	{
		if(*cszStr == '\n') //go to next line
		{
			if(nWidth > nLongest) //copy longest, if it's longer than previous longest
				nLongest = nWidth;
			nWidth = 0; //start from 0, but one more height
			nHeight++;
		}
		else
			nWidth++; //one more char wide

		cszStr++;
	}

	*pnCharsWide = (nLongest > nWidth ? nLongest : nWidth); //copy longest
	*pnCharsTall = nHeight;                                 //and height
}