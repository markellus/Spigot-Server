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

char* Call(int argc, char** argv)
{
	StdOutRedirect stdoutRedirect(8192);

	stdoutRedirect.start();
	int iResult = main(argc, argv);
	stdoutRedirect.stop();

	if(iResult != 0)
	{
		return "ERROR-INPUT";
	}

	char* szBuffer = new char[8193];

	int nOutRead = stdoutRedirect.getBuffer(szBuffer, 8192);

	if (!nOutRead)
	{
		return "ERROR-IO";
	}
	szBuffer[nOutRead] = '\0';

	return szBuffer;
}
