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
// Opts.h -- definitions of the options of the game, and structures of same. //
// look in the .cpp file for defaults, etc. for these.                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef __OPTS_H__
#define __OPTS_H__


#pragma pack(push, 1) //make sure that the structures don't try to align themselves funnily

typedef struct _APPOPTS //app-wide opts
{
	BYTE yFPS;   //show fps
	BYTE yAudio; //enable audio
} APPOPTS, * PAPPOPTS;

typedef struct _GAMOPTS //game-specific opts
{
	BYTE yPlayers;     //how many players
	BYTE yStartHealth; //starting health
	BYTE yStartBomns;  //etc.
	BYTE yStartRadius; //...
	BYTE yStartDamage; //...
	BYTE yMaxHealth;   //obviously
	BYTE yMaxBomns;    // .
	BYTE yMaxRadius;   // .
	BYTE yMaxDamage;   // .
	BYTE yMinRadius;   // .
	BYTE yMinDamage;   // .
	WORD wTimeLimit;   //game time limit (0 = none)
	BYTE yMaxBomnsOut; //how many bomns each player can drop at at time
	BYTE yInvulTime;   //how long invuls last
	BYTE yBomnTimer;   //how long each bomn stays before exploding
	BYTE yPunchDamage; //how much damage players do smacking into each other
	BYTE yHealthPerPowerup; //how much health each health gives
	BYTE yPersistentPowerups; //powerups are destroyed by explosions or not
} GAMOPTS, * PGAMOPTS;

typedef struct _LEVOPTS //level-specific options
{
	WORD wSpawnFreq;   //how often to check for spawning
	BYTE ySpawnChance; //how much chance (0-100%) of spawning at each check
	BYTE ySpawnBomns;  //relative bomn spawn number
	BYTE ySpawnHealth; //etc.
	BYTE ySpawnInvul;
	BYTE ySpawnRadUp;
	BYTE ySpawnRadDown;
	BYTE ySpawnDamUp;
	BYTE ySpawnDamDown;
} LEVOPTS, * PLEVOPTS;

typedef struct _GENOPTS //level generation options
{
	DWORD dwSeed; //0 for random
	WORD  wNormalWalls; //number of normal walls
	WORD  wIndestWalls; //etc.
	WORD  wWarps;
	WORD  wBomns;
	WORD  wHealths;
	WORD  wInvuls;
	WORD  wRadUps;
	WORD  wRadDowns;
	WORD  wDamUps;
	WORD  wDamDowns; //it's not hard, people
} GENOPTS, * PGENOPTS;

typedef struct _KEYCONFIG //key configuration (not actually stored as a file)
{
	BYTE yP1Up;   //each player's keys
	BYTE yP1Down;
	BYTE yP1Left;
	BYTE yP1Right;
	BYTE yP1Drop;

	BYTE yP2Up;
	BYTE yP2Down;
	BYTE yP2Left;
	BYTE yP2Right;
	BYTE yP2Drop;

	BYTE yP3Up;
	BYTE yP3Down;
	BYTE yP3Left;
	BYTE yP3Right;
	BYTE yP3Drop;

	BYTE yP4Up;
	BYTE yP4Down;
	BYTE yP4Left;
	BYTE yP4Right;
	BYTE yP4Drop;
} KEYCONFIG, * PKEYCONFIG;

#pragma pack(pop) //reset pack value to what it was before we called it above


BOOL GrabOpts(); //grab all options from files, creating default ones if they don't exist
BOOL SetAppOpts(PAPPOPTS); //dump app opts to file
BOOL SetGamOpts(PGAMOPTS); //dump game opts
BOOL SetLevOpts(PLEVOPTS); //same
BOOL SetGenOpts(PGENOPTS);
VOID GetAppOpts(PAPPOPTS); //get app opts (doesn't read the files--grabopts does that)
VOID GetGamOpts(PGAMOPTS); //get game opts
VOID GetLevOpts(PLEVOPTS); //same
BOOL GetGenOpts(PGENOPTS); //this actually reads the file, as the gen opts aren't stored in opts.cpp
VOID GetKeyConfig(PKEYCONFIG);//get key config (no file reading)
BOOL CreateDefaultAppOpts();//does what it says
BOOL CreateDefaultGamOpts();
BOOL CreateDefaultLevOpts();
BOOL CreateDefaultGenOpts();
BOOL CreateDefaultKeyConfig();


#endif