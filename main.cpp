/*
 * This file is part of De.Markellus.Math (https://github.com/markellus/Spigot-Server).
 * Copyright (c) 2019 Marcel Bulla.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <windows.h>
#include <SDKDDKVer.h>
#include "exports.h"
#include <iostream>

#pragma comment(lib, "spgt-lib.lib")

const int CONTENT_SIZE = 4096;

struct SM
{
	int iStatus;
	int iContentSizeMax = CONTENT_SIZE;
	int iContentSize;
	char cContent[CONTENT_SIZE];
};

enum Status
{
	EXIT = -1,
	RECEIVE_DATA = 100,
	RECEIVE_DATA_PART = 101,
	RECEIVE_DATA_RECEIVED = 102,
	SEND_DATA = 200,
	SEND_DATA_PART = 201,
	SEND_DATA_RECEIVED = 202
};

void SendData(SM* sm, HANDLE mutex, std::string strData)
{
	unsigned int iPos = 0;
	unsigned int iDataSize = strData.size();
	const char* data = strData.c_str();

	while(iPos != iDataSize)
	{
		WaitForSingleObject(mutex, INFINITE);

		if (sm->iStatus == RECEIVE_DATA_RECEIVED)
		{
			if (CONTENT_SIZE > iDataSize - iPos)
			{
				sm->iContentSize = iDataSize - iPos;
				memcpy(sm->cContent, data + sizeof(char) * iPos, iDataSize - iPos);
				sm->iStatus = RECEIVE_DATA;
				iPos += iDataSize - iPos;
			}
			else
			{
				iPos += CONTENT_SIZE;
				sm->iContentSize = CONTENT_SIZE;
				memcpy(sm->cContent, data + sizeof(char) * iPos, CONTENT_SIZE);
				sm->iStatus = RECEIVE_DATA_PART;
			}
		}

		ReleaseMutex(mutex);
	}
}

std::string ReceiveData(SM* sm, HANDLE mutex)
{
	bool bReceived = false;

	std::string strResult;

	while(!bReceived)
	{
		WaitForSingleObject(mutex, INFINITE);

		if (sm->iStatus != SEND_DATA_RECEIVED)
		{
			char* cPart = new char[sm->iContentSize + 1];
			memcpy(cPart, sm->cContent, sm->iContentSize);
			cPart[sm->iContentSize] = '\0';
			strResult += cPart;
			delete[] cPart;

			if (sm->iStatus == SEND_DATA)
			{
				bReceived = true;
			}

			sm->iStatus = SEND_DATA_RECEIVED;
		}

		ReleaseMutex(mutex);
	}

	return strResult;
}

bool parentProcessRunning(DWORD pid)
{
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
}

//Arguments:
// 0 - file path
// 1 - shared mutex name
// 2 - file mapping name
int main(int argc, char** argv)
{
	DWORD pidParent = 0;

	if(argc < 3)
	{
		return -1;
	}
	if(argc >= 4)
	{
		pidParent = atoi(argv[3]);
	}

	std::string strArgs = "-d 600";

	if(argc == 5)
	{
		strArgs = argv[4];
	}

	HANDLE fm;
	HANDLE mutex = CreateMutex(nullptr, FALSE, argv[1]);
	SM* sm;

	

	try
	{
		fm = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SM), argv[2]);
		sm = static_cast<struct SM*>(MapViewOfFile(fm, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(SM)));
		sm->iStatus = 0;
		sm->iContentSizeMax = CONTENT_SIZE;
	}
	catch (...)
	{
		return -2;
	}

	while(sm->iStatus != EXIT)
	{
		if(pidParent != 0 && !parentProcessRunning(pidParent))
		{
			sm->iStatus = EXIT;
		}
		else if (sm->iStatus == SEND_DATA || sm->iStatus == SEND_DATA_PART)
		{
			char* szData = _strdup(ReceiveData(sm, mutex).c_str());
			char* szArgs = _strdup(strArgs.c_str());

			char* szContent[3] = {nullptr};
			szContent[1] = szArgs;
			szContent[2] = szData;

			const std::string strOut = call(3, szContent);
			SendData(sm, mutex, strOut);

			delete szArgs;
			delete szData;
		}
		else
		{
			WaitForSingleObject(mutex, INFINITE);
			ReleaseMutex(mutex);
		}
	}
	UnmapViewOfFile(sm);
	CloseHandle(fm);
	ReleaseMutex(mutex);

	return 0;
}