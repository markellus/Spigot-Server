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

#ifndef SPGTINSTANCE_H
#define SPGTINSTANCE_H
#include <mutex>
#include <Windows.h>

using namespace std;

typedef char* (WINAPI *libparse) (char*, int);

class SpgtInstance
{
public:
	SpgtInstance();
	virtual ~SpgtInstance();

	bool isAvailable();
	char* parse(char* expression, int digitlimit);

private:
	string m_strTempDir;
	string m_strTempLib;
	mutex m_mutLock;
	HMODULE m_hndLibrary;
	libparse mp_libparse;
};

#endif