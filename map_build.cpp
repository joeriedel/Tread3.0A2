///////////////////////////////////////////////////////////////////////////////
// map_build.cpp
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
#include "TreadDoc.h"
#include "map_build.h"

static int c_build_count = 0;

// Runs a non-console process.
OS_FNEXP bool RunProcess(const char *exe, const char* cmdline, const char* wkdir)
{
	BOOL bProc;
	PROCESS_INFORMATION pInfo;
	STARTUPINFO sInfo;

	memset(&sInfo, 0, sizeof(STARTUPINFO));
	sInfo.cb = sizeof(STARTUPINFO);

	bProc = CreateProcess((char*)exe, (char*)cmdline, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, 
		NULL, wkdir, &sInfo, &pInfo);

	if(!bProc)
	{
		Sys_DisplayWindowsError(GetLastError());
		return(false);
	}

	return(true);
}

// Runs a build process.
OS_FNEXP bool RunConsoleProcess(const char *exe, const char* cmdline, const char* wkdir, CTreadDoc *doc, volatile bool *abort)
{
	char data[256], filename[256];
	BOOL bProc, bOk;
	HANDLE hReadFile, hWriteFile;
	DWORD status;
	PROCESS_INFORMATION pInfo;
	STARTUPINFO sInfo;
	DWORD nBytesRead;
	SECURITY_ATTRIBUTES sa;

	sprintf(filename, "c:\\temp\\tread_temp_build_file_%d.txt", c_build_count++);
	CreateDirectoryPathNative( "c:\\temp" );

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;
	
	hWriteFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL , NULL);

	hReadFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hWriteFile == INVALID_HANDLE_VALUE || hReadFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hWriteFile);
		CloseHandle(hReadFile);
		DeleteFile(filename);
		if (doc) doc->WriteToCompileWindow( "Child I/O failure.\n" );
		return(false);
	}
	
	memset(&sInfo, 0, sizeof(STARTUPINFO));
	sInfo.cb = sizeof(STARTUPINFO);
	sInfo.dwFlags = STARTF_USESTDHANDLES;
	sInfo.hStdError  = GetStdHandle(STD_ERROR_HANDLE);
	sInfo.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
	sInfo.hStdOutput = hWriteFile;

	bProc = CreateProcess((char*)exe, (char*)cmdline, NULL, NULL, TRUE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, 
		NULL, wkdir, &sInfo, &pInfo);

	if(!bProc)
	{
		Sys_DisplayWindowsError(GetLastError());
		CloseHandle(hWriteFile);
		CloseHandle(hReadFile);
		if (doc) doc->WriteToCompileWindow( "Unable to connect to process.\n" );
		return(false);
	}

	for(;;)
	{
		if( abort && *abort )
		{
			GetExitCodeProcess(pInfo.hProcess, &status);
			if(status == STILL_ACTIVE) // Nope, die.
			{
				TerminateProcess( pInfo.hProcess, 0 );
			}

			if (doc) doc->WriteToCompileWindow("\n\nCompilation Aborted.\n");
			break;
		}

		// Read.
		memset(data, 0, 256);
		FlushFileBuffers(hWriteFile);
		bOk = ReadFile(hReadFile, data, 255, &nBytesRead, NULL);
		if(bOk && nBytesRead == 0)
		{
			// EOF, Process running?
			GetExitCodeProcess(pInfo.hProcess, &status);
			if(status != STILL_ACTIVE) // Nope, die.
				break;

			Sleep(500);
			continue;
		}
		
		// Print out what we got.
		data[nBytesRead] = '\0';
		if (doc) doc->WriteToCompileWindow( data );
		Sleep(0);
	}

	CloseHandle(hWriteFile);
	CloseHandle(hReadFile);
	DeleteFile(filename);
	return(true);
}