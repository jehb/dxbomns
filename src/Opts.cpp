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
// Opts.cpp -- handles getting options from files and distributing them to   //
// wherever they're needed.                                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>

#include "DXBomns.h"
#include "Input.h"
#include "Opts.h"


APPOPTS     g_aoApp;   //these are read in and just copied when needed
GAMOPTS     g_goGame;  //note that the gen opts aren't here... they're
LEVOPTS     g_loLevel; //only loaded when needed
KEYCONFIG   g_kcKeys;


//gets the newest version of the files, creating defaults if necessary, and copies
//the data from the files into the global opts, so you can call the function
//to get the appropriate opt set and have the latest version
//returns whether it succeeds
BOOL GrabOpts()
{
	//open appopts.dat
	HANDLE hFile = CreateFile(".\\appopts.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		if(!CreateDefaultAppOpts()) //if it doesn't exist, create a default one
			return FALSE;
		
		//open it
		hFile = CreateFile(".\\appopts.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(!hFile || hFile == INVALID_HANDLE_VALUE)
		{
			Error("CreateFile", "unable to open appopts.dat");
			return FALSE;
		}
	}

	//read in the appopts struct
	DWORD dwSize = sizeof(g_aoApp);
	if(!ReadFile(hFile, &g_aoApp, sizeof(g_aoApp), &dwSize, NULL) || dwSize != sizeof(g_aoApp))
	{
		CloseHandle(hFile);
		Error("ReadFile", "unable to read appopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	//open gamopts.dat
	hFile = CreateFile(".\\gamopts.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		if(!CreateDefaultGamOpts()) //create a new one if it didn't exist
			return FALSE;
		
		//open the default
		hFile = CreateFile(".\\gamopts.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(!hFile || hFile == INVALID_HANDLE_VALUE)
		{
			Error("CreateFile", "unable to open gamopts.dat");
			return FALSE;
		}
	}

	//read in game opts
	dwSize = sizeof(g_goGame);
	if(!ReadFile(hFile, &g_goGame, sizeof(g_goGame), &dwSize, NULL) || dwSize != sizeof(g_goGame))
	{
		CloseHandle(hFile);
		Error("ReadFile", "unable to read gamopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	//open levopts.dat
	hFile = CreateFile(".\\levopts.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		if(!CreateDefaultLevOpts()) //create a new one if it didn't exist
			return FALSE;

		//then open it
		hFile = CreateFile(".\\levopts.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(!hFile || hFile == INVALID_HANDLE_VALUE)
		{
			Error("CreateFile", "unable to open levopts.dat");
			return FALSE;
		}
	}

	//read in levopts
	dwSize = sizeof(g_loLevel);
	if(!ReadFile(hFile, &g_loLevel, sizeof(g_loLevel), &dwSize, NULL) || dwSize != sizeof(g_loLevel))
	{
		CloseHandle(hFile);
		Error("ReadFile", "unable to read levopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	if(GetFileAttributes(".\\keys.cfg") == (DWORD)-1) //if keys.cfg doesn't exist, create a new one
	{
		if(!CreateDefaultKeyConfig())
			return FALSE;
	}
	for(INT i = 0; i < 4; i++) //for each player
	{
		CHAR szApp[] = {'p', 'l', 'a', 'y', 'e', 'r', ' ', '0' + i + 1, '\0'}; //the appropriate section
		CHAR szOut[1024];
		GetPrivateProfileString(szApp, "up", "up", szOut, 1024, ".\\keys.cfg"); //get the string
		((PBYTE)&g_kcKeys)[0 + i * 5] = StringToKey(szOut, KEY_UP);             //set the appropriate byte in the key config
		GetPrivateProfileString(szApp, "down", "down", szOut, 1024, ".\\keys.cfg");
		((PBYTE)&g_kcKeys)[1 + i * 5] = StringToKey(szOut, KEY_DOWN);
		GetPrivateProfileString(szApp, "left", "left", szOut, 1024, ".\\keys.cfg");
		((PBYTE)&g_kcKeys)[2 + i * 5] = StringToKey(szOut, KEY_LEFT);
		GetPrivateProfileString(szApp, "right", "right", szOut, 1024, ".\\keys.cfg");
		((PBYTE)&g_kcKeys)[3 + i * 5] = StringToKey(szOut, KEY_RIGHT);
		GetPrivateProfileString(szApp, "drop", "rcontrol", szOut, 1024, ".\\keys.cfg");
		((PBYTE)&g_kcKeys)[4 + i * 5] = StringToKey(szOut, KEY_RCONTROL);
	}

	return TRUE;
}

//clips the provided opts to defaults, then saves it to the file
//returns whether it succeeds
BOOL SetAppOpts(PAPPOPTS pao)
{
	if(!pao)
		return TRUE;
	
	if(pao->yFPS > 1) //clip
		pao->yFPS = 1;
	if(pao->yAudio > 1)
		pao->yAudio = 1;

	//open file
	HANDLE hFile = CreateFile(".\\appopts.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create appopts.dat");
		return FALSE;
	}

	//write
	DWORD dwSize = sizeof(APPOPTS);
	if(!WriteFile(hFile, pao, sizeof(APPOPTS), &dwSize, NULL) || dwSize != sizeof(APPOPTS))
	{
		CloseHandle(hFile);
		Error("WriteFile", "unable to write to appopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//clips the provided opts to defaults, then saves it to the file
//returns whether it succeeds
BOOL SetGamOpts(PGAMOPTS pgo)
{
	if(!pgo)
		return TRUE;

	if(pgo->yPlayers > 4)      //clip to defaults
		pgo->yPlayers = 4;
	if(pgo->yMaxHealth > 10)
		pgo->yMaxHealth = 10;
	if(pgo->yMaxHealth < 1)
		pgo->yMaxHealth = 1;
	if(pgo->yMaxBomns > 99)
		pgo->yMaxBomns = 99;
	if(pgo->yMaxRadius > 64)
		pgo->yMaxRadius = 64;
	if(pgo->yMaxDamage > 10)
		pgo->yMaxDamage = 10;
	if(pgo->yMinRadius > pgo->yMaxRadius)
		pgo->yMinRadius = pgo->yMaxRadius;
	if(pgo->yMinDamage > pgo->yMaxDamage)
		pgo->yMinDamage = pgo->yMaxDamage;
	if(pgo->yStartHealth > pgo->yMaxHealth) //still clippin' to defaults
		pgo->yStartHealth = pgo->yMaxHealth;
	if(pgo->yStartBomns > pgo->yMaxBomns)
		pgo->yStartBomns = pgo->yMaxBomns;
	if(pgo->yStartRadius > pgo->yMaxRadius)
		pgo->yStartRadius = pgo->yMaxRadius;
	if(pgo->yStartRadius < pgo->yMinRadius)
		pgo->yStartRadius = pgo->yMinRadius;
	if(pgo->yStartDamage > pgo->yMaxDamage)
		pgo->yStartDamage = pgo->yMaxDamage;
	if(pgo->yStartDamage < pgo->yMinDamage)
		pgo->yStartDamage = pgo->yMinDamage;
	if(pgo->yMaxBomnsOut > 10)
		pgo->yMaxBomnsOut = 10;
	if(pgo->yInvulTime > 60)
		pgo->yInvulTime = 60;
	if(pgo->yBomnTimer > 10)
		pgo->yBomnTimer = 10;
	if(pgo->yPunchDamage > 10)
		pgo->yPunchDamage = 10;
	if(pgo->yHealthPerPowerup > 10)
		pgo->yHealthPerPowerup = 10;
	if(pgo->yPersistentPowerups > 1)
		pgo->yPersistentPowerups = 1; //phew!

	//open the file
	HANDLE hFile = CreateFile(".\\gamopts.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create gamopts.dat");
		return FALSE;
	}

	//write to it
	DWORD dwSize = sizeof(GAMOPTS);
	if(!WriteFile(hFile, pgo, sizeof(GAMOPTS), &dwSize, NULL) || dwSize != sizeof(GAMOPTS))
	{
		CloseHandle(hFile);
		Error("WriteFile", "unable to write to gamopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//clips the provided opts to defaults, then saves it to the file
//returns whether it succeeds
BOOL SetLevOpts(PLEVOPTS plo)
{
	if(!plo)
		return TRUE;

	if(plo->ySpawnChance > 100) //clip to defaults
		plo->ySpawnChance = 100;
	if(plo->ySpawnChance < 1)
		plo->ySpawnChance = 1;

	//open the file
	HANDLE hFile = CreateFile(".\\levopts.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create levopts.dat");
		return FALSE;
	}

	//write
	DWORD dwSize = sizeof(LEVOPTS);
	if(!WriteFile(hFile, plo, sizeof(LEVOPTS), &dwSize, NULL) || dwSize != sizeof(LEVOPTS))
	{
		CloseHandle(hFile);
		Error("WriteFile", "unable to write to levopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//saves gen opts to a file
//returns whether it succeeds
BOOL SetGenOpts(PGENOPTS pgo)
{
	if(!pgo)
		return TRUE;

	//open the file
	HANDLE hFile = CreateFile(".\\genopts.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create genopts.dat");
		return FALSE;
	}

	//write to it
	DWORD dwSize = sizeof(GENOPTS);
	if(!WriteFile(hFile, pgo, sizeof(GENOPTS), &dwSize, NULL) || dwSize != sizeof(GENOPTS))
	{
		CloseHandle(hFile);
		Error("WriteFile", "unable to write to genopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//copies the version of the appropriate opts, gotten from GrabOpts, to the
//provided opts
VOID GetAppOpts(PAPPOPTS pao)
{
	if(!pao)
		return;

	CopyMemory(pao, &g_aoApp, sizeof(APPOPTS));
}

//copies the version of the appropriate opts, gotten from GrabOpts, to the
//provided opts
VOID GetGamOpts(PGAMOPTS pgo)
{
	if(!pgo)
		return;

	CopyMemory(pgo, &g_goGame, sizeof(GAMOPTS));
}

//copies the version of the appropriate opts, gotten from GrabOpts, to the
//provided opts
VOID GetLevOpts(PLEVOPTS plo)
{
	if(!plo)
		return;

	CopyMemory(plo, &g_loLevel, sizeof(LEVOPTS));
}

//reads the gen opts from a file, and returns them
//returns whether it succeeds
BOOL GetGenOpts(PGENOPTS pgo)
{
	if(!pgo)
		return TRUE;

	//open the file
	HANDLE hFile = CreateFile(".\\genopts.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		if(!CreateDefaultGenOpts()) //create a default if it didn't exist
			return FALSE;
		
		//then open that
		hFile = CreateFile(".\\genopts.dat", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(!hFile || hFile == INVALID_HANDLE_VALUE)
		{
			Error("CreateFile", "unable to open genopts.dat");
			return FALSE;
		}
	}

	//read it in
	DWORD dwSize = sizeof(GENOPTS);
	if(!ReadFile(hFile, pgo, sizeof(GENOPTS), &dwSize, NULL) || dwSize != sizeof(GENOPTS))
	{
		CloseHandle(hFile);
		Error("ReadFile", "unable to read genopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//copies the version of the appropriate opts, gotten from GrabOpts, to the
//provided opts
VOID GetKeyConfig(PKEYCONFIG pkc)
{
	if(!pkc)
		return;

	CopyMemory(pkc, &g_kcKeys, sizeof(KEYCONFIG));
}

//creates a default opts file
//returns whether it succeeds
BOOL CreateDefaultAppOpts()
{
	APPOPTS ao = {0, 1}; //these are the defaults

	//open the file
	HANDLE hFile = CreateFile(".\\appopts.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create a default appopts.dat");
		return FALSE;
	}

	//write to it
	DWORD dwSize = sizeof(ao);
	if(!WriteFile(hFile, &ao, sizeof(ao), &dwSize, NULL) || dwSize != sizeof(ao))
	{
		CloseHandle(hFile);
		Error("WriteFile", "unable to write a default appopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//creates a default opts file
//returns whether it succeeds
BOOL CreateDefaultGamOpts()
{
	GAMOPTS go = {2, 10, 1, 8, 2, 10, 99, 64, 10, 0, 0, 0, 2, 10, 5, 1, 2, 0}; //these are the defaults

	//open the file
	HANDLE hFile = CreateFile(".\\gamopts.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create a default gamopts.dat");
		return FALSE;
	}

	//write to it
	DWORD dwSize = sizeof(go);
	if(!WriteFile(hFile, &go, sizeof(go), &dwSize, NULL) || dwSize != sizeof(go))
	{
		CloseHandle(hFile);
		Error("WriteFile", "unable to write a default gamopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//creates a default opts file
//returns whether it succeeds
BOOL CreateDefaultLevOpts()
{
	LEVOPTS lo = {2, 75, 255, 255, 255, 255, 0, 255, 0}; //these are the defaults

	//open the file
	HANDLE hFile = CreateFile(".\\levopts.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create a default levopts.dat");
		return FALSE;
	}

	//write to it
	DWORD dwSize = sizeof(lo);
	if(!WriteFile(hFile, &lo, sizeof(lo), &dwSize, NULL) || dwSize != sizeof(lo))
	{
		CloseHandle(hFile);
		Error("WriteFile", "unable to write a default levopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//creates a default opts file
//returns whether it succeeds
BOOL CreateDefaultGenOpts()
{
	GENOPTS go = {0, 800, 400, 4, 80, 40, 20, 80, 0, 40, 0}; //these are the defaults

	//open the file
	HANDLE hFile = CreateFile(".\\genopts.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		Error("CreateFile", "unable to create a default genopts.dat");
		return FALSE;
	}

	//write to it
	DWORD dwSize = sizeof(go);
	if(!WriteFile(hFile, &go, sizeof(go), &dwSize, NULL) || dwSize != sizeof(go))
	{
		CloseHandle(hFile);
		Error("WriteFile", "unable to write a default genopts.dat");
		return FALSE;
	}
	CloseHandle(hFile);

	return TRUE;
}

//creates a default key config file
//returns whether it succeeds
BOOL CreateDefaultKeyConfig()
{
	WritePrivateProfileString("player 1", "up", "up", ".\\keys.cfg");          //write the deals
	WritePrivateProfileString("player 1", "down", "down", ".\\keys.cfg");
	WritePrivateProfileString("player 1", "left", "left", ".\\keys.cfg");
	WritePrivateProfileString("player 1", "right", "right", ".\\keys.cfg");
	WritePrivateProfileString("player 1", "drop", "rcontrol", ".\\keys.cfg");

	WritePrivateProfileString("player 2", "up", "w", ".\\keys.cfg");
	WritePrivateProfileString("player 2", "down", "s", ".\\keys.cfg");
	WritePrivateProfileString("player 2", "left", "a", ".\\keys.cfg");
	WritePrivateProfileString("player 2", "right", "d", ".\\keys.cfg");
	WritePrivateProfileString("player 2", "drop", "lshift", ".\\keys.cfg");

	WritePrivateProfileString("player 3", "up", "i", ".\\keys.cfg");
	WritePrivateProfileString("player 3", "down", "k", ".\\keys.cfg");
	WritePrivateProfileString("player 3", "left", "j", ".\\keys.cfg");
	WritePrivateProfileString("player 3", "right", "l", ".\\keys.cfg");
	WritePrivateProfileString("player 3", "drop", "space", ".\\keys.cfg");

	WritePrivateProfileString("player 4", "up", "numpad8", ".\\keys.cfg");
	WritePrivateProfileString("player 4", "down", "numpad5", ".\\keys.cfg");
	WritePrivateProfileString("player 4", "left", "numpad4", ".\\keys.cfg");
	WritePrivateProfileString("player 4", "right", "numpad6", ".\\keys.cfg");
	WritePrivateProfileString("player 4", "drop", "numpad0", ".\\keys.cfg");

	return TRUE;
}