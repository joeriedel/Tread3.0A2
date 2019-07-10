///////////////////////////////////////////////////////////////////////////////
// Plugin.cpp
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Joe Riedel
// All rights reserved.
//
// Redistribution and use in source and binary forms, 
// with or without modification, are permitted provided 
// that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
//
// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation and/or 
// other materials provided with the distribution. 
//
// Neither the name of the <ORGANIZATION> nor the names of its contributors may be 
// used to endorse or promote products derived from this software without specific 
// prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
// OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Quake.h"
#include "Quake2.h"
#include "Quake3.h"

class CQuakeGames : public CPlugin
{
public:

	virtual void Release()
	{
		delete this;
	}

	virtual CPluginGame *GamePlugin(int i)
	{
		switch (i)
		{
		case 0:
			return new CQuakeGame();
		case 1:
			return new CQuake2Game();
		case 2:
			return new CQuake3Game();
		}
		return 0;
	}

	virtual CPluginFileImport *FileImporter(int i)
	{
		switch (i)
		{
		case 0:
			return new CQuakeMap();
		case 1:
			return new CQuake2Map();
		case 2:
			return new CQuake3Map();
		}
		return 0;
	}

	virtual CObjectCreator *ObjectCreator(int i)
	{
		return 0;
	}

	virtual CMapObject *ObjectForClass(int classbits, int subclassbits)
	{
		return 0;
	}
};

OS_PLUGIN_EXPORT_FN int PluginAPIVersion()
{
	return TREAD_API_VERSION;
}

OS_PLUGIN_EXPORT_FN CPlugin *PluginCreate(int i)
{
	switch (i)
	{
	case 0:
		return new CQuakeGames();
	}
	return 0;
}

class CQGamesApp : public CWinApp
{
public:
	CQGamesApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

// CQGamesApp

BEGIN_MESSAGE_MAP(CQGamesApp, CWinApp)
END_MESSAGE_MAP()

// CQGamesApp construction

CQGamesApp::CQGamesApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CQuake2App object

CQGamesApp theApp;


// CQGamesApp initialization

BOOL CQGamesApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}