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
// Sound.cpp -- the sound library class that's used by everything DX in      //
// DXBomns.  It's just a wrapper around the DSound API, but if you wanted    //
// to port DXBomns to some OTHER API, it'd be a simple matter of rewriting   //
// this class.  Anyway, it's pretty simply laid out.                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <mmsystem.h>

#include <string.h>

#include "DXBomns.h"
#include "XNotDirect.h"
#include "Opts.h"
#include "Sound.h"

#pragma comment(lib, "dsound")


extern HANDLE g_hHeap; //from DXBomns.cpp


BOOL InitSoundSystem(PSOUND_VARS, HWND);                                //various functions that
VOID ReleaseSoundSystem(PSOUND_VARS);                                   //are only useful in this
IDirectSoundBuffer8 * CreateSoundBufferFromFile(IDirectSound8 *, PSTR); //module.  See below for
BOOL ReloadSoundBufferFromFile(IDirectSoundBuffer8 *, PSTR);            //more info


APPOPTS g_aoSound; //for sound enabling/disabling


//initializes the sound system into a sound vars struct
//returns whether it succeeds
BOOL InitSoundSystem(PSOUND_VARS psv, HWND hWnd)
{
	if(!psv)
		return FALSE;

	if(DirectSoundCreate8(NULL, &psv->pDS, NULL) != DS_OK) //create the dsound object
	{
		Error("DirectSoundCreate8", "unable to create a DirectSound interface");
		return FALSE;
	}

	if(psv->pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY) != DS_OK) //set coop level
	{
		Error("IDirectSound8::SetCooperativeLevel", "unable to set the coop level of the sound");
		return FALSE;
	}

	DSBUFFERDESC dsbd = {sizeof(DSBUFFERDESC), DSBCAPS_PRIMARYBUFFER, 0, 0, NULL, 0}; //get the primary buffer so we can set the format
	IDirectSoundBuffer * pdsbPrimary = NULL;
	if(psv->pDS->CreateSoundBuffer(&dsbd, &pdsbPrimary, NULL) != DS_OK)
	{
		Error("IDirectSound8::CreateSoundBuffer", "unable to load the primary buffer to set its format");
		return FALSE;
	}

	WAVEFORMATEX wfex = {WAVE_FORMAT_PCM, 2, 11025, 44100, 4, 16, 0}; //stereo, 11025 Hz, 16 bit
	if(pdsbPrimary->SetFormat(&wfex) != DS_OK) //set the primary buffer to that format
	{
		SAFERELEASE(pdsbPrimary)
		Error("IDirectSoundBuffer::SetFormat", "unable to set the format of the primary buffer");
		return FALSE;
	}
	SAFERELEASE(pdsbPrimary) //we're done with it

	return TRUE;
}

//deallocates anything allocated
VOID ReleaseSoundSystem(PSOUND_VARS psv)
{
	if(psv)
	{
		SAFERELEASE(psv->pDS)
	}
}

//creates a dsound buffer from a file... doesn't actually load it, as
//that's what ReloadSoundBufferFromFile does.  Returns null if it fails
IDirectSoundBuffer8 * CreateSoundBufferFromFile(IDirectSound8 * pDS, PSTR szFile)
{
	HMMIO hmmio = mmioOpen(szFile, NULL, MMIO_ALLOCBUF | MMIO_READ); //open the file
	if(!hmmio)
	{
		Error("mmioOpen", "unable to open a specified wave file");
		return NULL;
	}

	MMCKINFO ckRiff;
	ZeroMemory(&ckRiff, sizeof(ckRiff));
	ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if(mmioDescend(hmmio, &ckRiff, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR) //find the 'WAVE' chunk
	{
		mmioClose(hmmio, 0);
		Error("mmioDescend", "unable to descend into a specified wave file... is it really a wave?");
		return NULL;
	}

	MMCKINFO ckIn;
	ZeroMemory(&ckIn, sizeof(ckIn));
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if(mmioDescend(hmmio, &ckIn, &ckRiff, 0) != MMSYSERR_NOERROR) //find the 'fmt ' chunk
	{
		mmioClose(hmmio, 0);
		Error("mmioDescend", "unable to descent into a wave's format");
		return NULL;
	}
	
	WAVEFORMATEX wfex;
	ZeroMemory(&wfex, sizeof(wfex));
	if((DWORD)mmioRead(hmmio, (HPSTR)&wfex, sizeof(wfex)) != sizeof(wfex)) //read in the wave format
	{
		mmioClose(hmmio, 0);
		Error("mmioRead", "unable to read a wave's format");
		return NULL;
	}

	if(mmioAscend(hmmio, &ckIn, 0) != MMSYSERR_NOERROR) //go back to 'WAVE' chunk
	{
		mmioClose(hmmio, 0);
		Error("mmioAscend", "unable to get out of the format chunk");
		return NULL;
	}
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(mmioDescend(hmmio, &ckIn, &ckRiff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) //go down to 'data' chunk (we need the size to create the buffer)
	{
		mmioClose(hmmio, 0);
		Error("mmioDescend", "unable to find the wave data chunk");
		return NULL;
	}
	mmioClose(hmmio, 0);
	
	DSBUFFERDESC dsbd = {sizeof(DSBUFFERDESC), DSBCAPS_CTRLPAN, ckIn.cksize, 0, &wfex, 0}; //create the buffer
	IDirectSoundBuffer * pTemp    = NULL;
	if(pDS->CreateSoundBuffer(&dsbd, &pTemp, NULL) != DS_OK)
	{
		Error("IDirectSound8::CreateSoundBuffer", "unable to create a temporary sound object");
		return NULL;
	}
	IDirectSoundBuffer8 * pRet     = NULL;
	if(pTemp->QueryInterface(IID_IDirectSoundBuffer8, (PVOID *)&pRet) != S_OK) //get a ds8 buffer instead of a ds buffer
	{
		SAFEDELETE(pTemp)
		Error("IDirectSoundBuffer::QueryInterface", "unable to create an IDirectSoundBuffer8 interface");
		return NULL;
	}
	SAFERELEASE(pTemp)

	if(!ReloadSoundBufferFromFile(pRet, szFile)) //load that bitch (somewhat redundant because it just descends right back into the data chunk)
	{
		SAFERELEASE(pRet)
		return NULL;
	}
	
	return pRet;
}

//loads wave data into a wave buffer created with the above function.
//returns whether it succeeds
BOOL ReloadSoundBufferFromFile(IDirectSoundBuffer8 * pdsbOut, PSTR szFile)
{
	HMMIO hmmio = mmioOpen(szFile, NULL, MMIO_ALLOCBUF | MMIO_READ); //open it
	if(!hmmio)
	{
		Error("mmioOpen", "unable to open a specified wave file");
		return FALSE;
	}

	MMCKINFO ckRiff;
	ZeroMemory(&ckRiff, sizeof(ckRiff));
	ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if(mmioDescend(hmmio, &ckRiff, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR) //go to 'WAVE'
	{
		mmioClose(hmmio, 0);
		Error("mmioDescend", "unable to descend into a specified wave file... is it really a wave?");
		return FALSE;
	}

	MMCKINFO ckIn;
	ZeroMemory(&ckIn, sizeof(ckIn));
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(mmioDescend(hmmio, &ckIn, &ckRiff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) //go straight to 'data'
	{
		mmioClose(hmmio, 0);
		Error("mmioDescend", "unable to descend into the data of a wave file");
		return FALSE;
	}
	
	PBYTE pyData = (PBYTE)HeapAlloc(g_hHeap, 0, ckIn.cksize); //alloc
	if(!pyData)
	{
		mmioClose(hmmio, 0);
		Error("HeapAlloc", "not enough memory");
		return FALSE;
	}
	if((DWORD)mmioRead(hmmio, (HPSTR)pyData, ckIn.cksize) != ckIn.cksize) //read it in
	{
		HeapFree(g_hHeap, 0, pyData);
		mmioClose(hmmio, 0);
		Error("mmioRead", "unable to read the wave data");
		return FALSE;
	}
	mmioClose(hmmio, 0);

	PBYTE pyPtr1  = NULL; //to lock the deals
	DWORD dwSize1 = 0;
	PBYTE pyPtr2  = NULL;
	DWORD dwSize2 = 0;
	if(pdsbOut->Lock(0, 0, (PVOID *)&pyPtr1, &dwSize1, (PVOID *)&pyPtr2, &dwSize2, DSBLOCK_ENTIREBUFFER) != DS_OK) //lock it
	{
		HeapFree(g_hHeap, 0, pyData);
		Error("IDirectSoundBuffer8::Lock", "unable to lock the sound we're trying to load");
		return FALSE;
	}

	CopyMemory(pyPtr1, pyData, dwSize1); //copy to pointer 1
	if(pyPtr2)
		CopyMemory(pyPtr2, pyData + dwSize1, dwSize2); //copy to pointer 2
	HeapFree(g_hHeap, 0, pyData); //dealloc
	pyData = NULL;

	if(pdsbOut->Unlock(pyPtr1, dwSize1, pyPtr2, dwSize2) != DS_OK) //unlock it
	{
		Error("IDirectSoundBuffer8::Unlock", "unable to unlock the sound we just loaded");
		return FALSE;
	}

	if(pdsbOut->SetCurrentPosition(0) != DS_OK) //make sure we're at the beginning (play does this too, so this is totally pointless)
	{
		Error("IDirectSoundBuffer8::SetCurrentPosition", "unable to reset the wave's position");
		return FALSE;
	}

	return TRUE;
}


//creates a soundsystem object
//returns null if it fails
CSoundSystem * CreateSoundSystem(HWND hWnd)
{
	CSoundSystem * pRet = new CSoundSystem();
	if(!pRet)
	{
		Error("new CSoundSystem", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(hWnd))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}


//zero shits
CSoundSystem::CSoundSystem()
{
	ZeroMemory(&m_svInnards, sizeof(m_svInnards));
}

//dealloc
CSoundSystem::~CSoundSystem()
{
	ReleaseSoundSystem(&m_svInnards);
}

//initialize dsound into its innards if audio is enabled
//returns whether it succeeds
BOOL CSoundSystem::Initialize(HWND hWnd)
{
	GetAppOpts(&g_aoSound);
	if(g_aoSound.yAudio)
		return InitSoundSystem(&m_svInnards, hWnd);
	else
		return TRUE;
}

//creates a sound buffer from a wave file
//returns null if it fails
CSound * CSoundSystem::CreateSound(PCSTR cszFile)
{
	CSound * pRet = new CSound();
	if(!pRet)
	{
		Error("new CSound", "not enough memory");
		return NULL;
	}

	if(!pRet->Initialize(m_svInnards.pDS, cszFile))
	{
		SAFEDELETE(pRet)
		return NULL;
	}

	return pRet;
}


//zero everything
CSound::CSound()
{
	m_szFile = NULL;
	m_pdsbSound = NULL;
}

//dealloc anything
CSound::~CSound()
{
	Stop();
	SAFEDELETEARRAY(m_szFile)
	SAFERELEASE(m_pdsbSound)
}

//loads the sound into the file if audio is enabled
BOOL CSound::Initialize(IDirectSound8 * pDS, PCSTR cszFile)
{
	if(g_aoSound.yAudio)
	{
		m_szFile = new CHAR[strlen(cszFile) + 1]; //copy the filename
		if(!m_szFile)
		{
			Error("new CHAR []", "not enough memory");
			return FALSE;
		}
		strcpy(m_szFile, cszFile);

		m_pdsbSound = CreateSoundBufferFromFile(pDS, m_szFile); //create it
		if(!m_pdsbSound)
			return FALSE;
	}

	return TRUE;
}

//plays the wave file with a pan value.  nPan should be between 0 and 63,
//corresponding to which level tile the sound is playing from.  Also,
//it loops if bLoop is TRUE.  Returns whether it succeeds
BOOL CSound::PanPlay(INT nPan, BOOL bLoop)
{
	if(g_aoSound.yAudio)
	{
		if(nPan <  0) nPan =  0;
		if(nPan > 63) nPan = 63;
		if(m_pdsbSound->SetPan(20000l * nPan / 63l - 10000l) != DS_OK) //turn 0 to 63 into -10000 to 10000, pan it there
		{
			Error("IDirectSoundBuffer8::SetPan", "unable to pan a sound");
			return FALSE;
		}

		return Play(bLoop); //play it
	}
	else
		return TRUE;
}

//plays a wave file, looping if bLoop is true.  Note that if you've previously
//called PanPlay on this wave file, it'll STILL be at the previous pan...
//so, for each wave file, ONLY call EITHER Play or PanPlay.
//returns whether it was successful
BOOL CSound::Play(BOOL bLoop)
{
	if(g_aoSound.yAudio)
	{
		if(m_pdsbSound->SetCurrentPosition(0) != DS_OK) //make sure we're at the beginning
		{
			Error("IDirectSoundBuffer8::SetCurrentPosition", "unable to go to the beginning of a wave");
			return FALSE;
		}

		HRESULT hr = m_pdsbSound->Play(0, 0, (bLoop ? DSBPLAY_LOOPING : 0)); //play it
		if(hr == DSERR_BUFFERLOST) //if it was lost
		{
			if(m_pdsbSound->Restore() != DS_OK) //restore it
			{
				Error("IDirectSoundBuffer8::Restore", "unable to restore a sound after alt-tabbing");
				return FALSE;
			}
			if(!ReloadSoundBufferFromFile(m_pdsbSound, m_szFile)) //and reload it
				return FALSE;

			if(m_pdsbSound->Play(0, 0, (bLoop ? DSBPLAY_LOOPING : 0)) != DS_OK) //THEN play it
			{
				Error("IDirectSoundBuffer8::Play", "unable to play a reallocated sound");
				return FALSE;
			}
		}
		else if(hr != DS_OK) //if it failed for some other reason
		{
			Error("IDirectSoundBuffer8::Play", "unable to play a sound");
			return FALSE;
		}
	}

	return TRUE;
}

//stops the sound
VOID CSound::Stop()
{
	if(g_aoSound.yAudio)
	{
		if(m_pdsbSound)
			m_pdsbSound->Stop();
	}
}