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

struct SM
{
	int iStatus;
	int iArgc;
	char cContent[4096];
};

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
	}
	catch (...)
	{
		return -2;
	}

	while(sm->iStatus != -1)
	{
		WaitForSingleObject(mutex, INFINITE);
		if (sm->iStatus == 2)
		{
			char* cContent[2];
			cContent[1] = sm->cContent;
			std::string strOut = Call(sm->iArgc + 1, cContent);
			memcpy(sm->cContent, strOut.c_str(), strOut.size());
			sm->iStatus = 1;
		}
		else
		{
			Sleep(1);
		}
		ReleaseMutex(mutex);
	}
	UnmapViewOfFile(sm);
	CloseHandle(fm);
	ReleaseMutex(mutex);

	return 0;
}