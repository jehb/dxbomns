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
// Level.h -- header for the level class, a quite extensive class used not   //
// only in the game, but also the editor dialog and fullscreen editor.  Look //
// to the .cpp file for more specific info.                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __LEVEL_H__
#define __LEVEL_H__


class CLevel;


#include "Graphics.h"
#include "Sound.h"
#include "Opts.h"


#define TILE_BLANK         0 //defines for tiles... 0 = blank
#define TILE_WALL_NORMAL   1 //normal wall
#define TILE_WALL_INDEST   2 //indestructible wall
#define TILE_WARP          3 //warp zones

#define TILE_BOMN          4 //bomn powerups
#define TILE_HEALTH        5 //health powerups
#define TILE_INVUL         6 //invulnerabilities
#define TILE_RAD_UP        7 //radius ups
#define TILE_RAD_DOWN      8 //downs
#define TILE_DAM_UP        9 //damage
#define TILE_DAM_DOWN     10 //down


#define LEVEL_WIDTH       64 //64 tiles across
#define LEVEL_HEIGHT      45 //45 tiles tall
#define LEVEL_SIZE      2880 //64 * 45 = 2880


#define MAKEOFFSET(x, y) (((INT)(x)) + (((INT)(y)) << 6)) //turn x,y (tiles, not pixels) into a linear offset
#define XFROMOFFSET(o)   (((INT)(o)) % LEVEL_WIDTH) //turn a linear level offset into x component
#define YFROMOFFSET(o)   (((INT)(o)) >> 6)          //and y component


typedef enum _LEVELTYPE //where is the level used?
{
	LT_GENERATEANDDUMP, //in the editor dialog
	LT_EDIT,            //in the fullscreen map editor
	LT_PLAY,            //in the game

	LT_FORCE_DWORD = 0x7fffffff
} LEVELTYPE, * PLEVELTYPE;


class CLevel //and now, the level class itself
{
	public:
		CLevel();
		~CLevel();

		BOOL Initialize(LEVELTYPE, CGraphics *, CImage *, CSoundSystem *); //initialize it

		VOID Clear();               //for all three leveltypes -- clear the level
		BOOL Generate();            //generate new stuff
		BOOL LoadFromFile(PCSTR);   //load
		BOOL DumpToFile(PCSTR);     //save
		BOOL GeneratePreview();     //create preview.dat

		VOID SetTile(UINT, BYTE);                //for LT_EDIT only -- set a tile
		VOID SetPlayerStart(UINT, BYTE, BYTE);   //set player start
		BYTE GetTile(UINT);                      //for anyone who wants it -- get a tile
		VOID GetPlayerStart(UINT, PBYTE, PBYTE); //anyone -- get a player start

		BOOL IsValid();             //for whoever wants it (only useful for LT_EDIT, though) -- is the level valid?

		BOOL Reset();               //for LT_EDIT and LT_PLAY -- reset the images

		BOOL StartGame();           //for LT_PLAY only -- on game start
		VOID PauseGame();           //on game pause
		VOID UnpauseGame();         //on game unpause

		BOOL Destroy(UINT, BOOL, BOOL);          //LT_PLAY -- destroy a tile
		UINT GetSpawnDestination(INT, INT, INT); //poorly named--it actually gets a WARP destination, but whatever

		BOOL Update();              //for LT_PLAY only -- each frame
		BOOL Render(CImage *);      //for LT_EDIT and LT_PLAY -- render it

	private:
		VOID DrawAll();                //set all draw flags
		UINT GetRandomEmptyTile(BOOL); //for anyone who wants to call it -- get the offset of a random empty tile
		BOOL Spawn(BYTE);              //LT_PLAY -- spawn an item somewhere in the level

		LEVELTYPE m_ltType;        //what type is it?

		BYTE m_ayTile[LEVEL_SIZE]; //the array of tiles
		BYTE m_ayPlayerStart[8];   //one for each player, x and y

		BYTE m_ayDraw[LEVEL_SIZE]; //draw flags for each tile

		LEVOPTS m_loLevOpts;       //a copy of the level options
		DWORD   m_dwNextSpawnTime; //next spawn item check time

		CImage       * m_piScreen; //blitted over the back buffer each frame

		CSound       * m_psCrumble;//crumble sound
		CSound       * m_psSpawn;  //spawn sound

		CGraphics    * m_pCopyGraphics; //copies of stuff
		CImage       * m_piCopySprites;
		CSoundSystem * m_pCopySoundSystem;

		///////////////////////////////////////////////////
		// to disable the copy ctor and assignment operator
		CLevel(const CLevel &);
		VOID operator = (const CLevel &);
};


CLevel * CreateLevel(LEVELTYPE, CGraphics *, CImage *, CSoundSystem *); //create a level


#endif