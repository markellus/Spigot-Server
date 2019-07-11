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

#include "exports.h"
#include "../spigot/main.cpp"
#include "StdOutRedirect.h"

#define REDIRECT_BUFFER_SIZE 8192

char* s_szBuffer = new char[REDIRECT_BUFFER_SIZE];

const char* ERR_INTERNAL = "ERROR-INTERNAL-BUFFER\0";
const char* ERR_INPUT = "ERROR-INPUT\0";
const char* ERR_IO = "ERROR-IO\0";

char* call(int argc, char** argv)
{
	StdOutRedirect stdoutRedirect(REDIRECT_BUFFER_SIZE - 1);

	const int iStart = stdoutRedirect.start();
	const int iResult = main(argc, argv);
	const int iStop = stdoutRedirect.stop();

	if(iStart == -1 || iStop == -1)
	{
		memcpy(s_szBuffer, ERR_INTERNAL, 22);
	}
	else if(iResult != 0)
	{
		memcpy(s_szBuffer, ERR_INPUT, 12);
	}
	else
	{
		const int nOutRead = stdoutRedirect.getBuffer(s_szBuffer, REDIRECT_BUFFER_SIZE - 1);

		if (!nOutRead)
		{
			memcpy(s_szBuffer, ERR_IO, 10);
		}
		else
		{
			s_szBuffer[nOutRead] = '\0';
		}
	}

	return s_szBuffer;
}
