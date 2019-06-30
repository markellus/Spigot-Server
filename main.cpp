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

char* ReceiveData(SM* sm, HANDLE mutex)
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

			if (sm->iStatus == SEND_DATA)
			{
				bReceived = true;
			}

			sm->iStatus = SEND_DATA_RECEIVED;
		}

		ReleaseMutex(mutex);
	}

	char* cResult = new char[strResult.size()];
	memcpy(cResult, strResult.c_str(), strResult.size() + 1);

	return cResult;
}

int main(int argc, char** argv)
{
	HANDLE fm;
	HANDLE mutex = CreateMutex(nullptr, FALSE, "SPGT_SM_MTX");
	SM* sm;

	try
	{
		fm = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SM), L"SPGT_SM");
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
		if (sm->iStatus == SEND_DATA || sm->iStatus == SEND_DATA_PART)
		{
			char* cContent[3];
			cContent[1] = "-d 600\0";
			cContent[2] = ReceiveData(sm, mutex);
			std::string strOut = Call(3, cContent);
			SendData(sm, mutex, strOut);
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