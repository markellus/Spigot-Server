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

#include "SpgtInstance.h"
#include <filesystem>
#include <string>

using namespace  std::experimental::filesystem;

int sg_iInstanceCount = 0;
int sg_iVersionCount = 0;

void dummy(){}

SpgtInstance::SpgtInstance()
  : m_strTempDir(temp_directory_path().string() + "\\spgt-instance" + to_string(sg_iInstanceCount++))
  , m_mutLock()
  , m_hndLibrary(nullptr)
  , mp_libparse()
{
	create_directory(m_strTempDir);

	m_strTempLib = m_strTempDir + "\\spgt" + to_string(sg_iVersionCount) + "-instance" + to_string(sg_iInstanceCount - 1) + ".dll";

	char path[MAX_PATH];
	HMODULE hm = NULL;

	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCSTR>(&dummy), &hm) == 0)
	{
		const int ret = GetLastError();
		exit(ret);
	}
	if (GetModuleFileName(hm, path, sizeof(path)) == 0)
	{
		const int ret = GetLastError();
		exit(ret);
	}

	if (!exists(m_strTempLib))
	{
		string strLibSource = string(path);
		strLibSource.replace(strLibSource.find("wrp.dll"), string("wrp.dll").size(), "lib.dll");
		FILE* source = fopen(strLibSource.c_str(), "rb");
		FILE* dest = fopen(m_strTempLib.c_str(), "wb");

		char buf[1024];
		size_t size;

		while ((size = fread(buf, 1, BUFSIZ, source))) {
			fwrite(buf, 1, size, dest);
		}

		fclose(source);
		fclose(dest);
	}

	m_hndLibrary = LoadLibrary(m_strTempLib.c_str());
	mp_libparse = reinterpret_cast<libparse>(GetProcAddress(m_hndLibrary, "parse"));
}

SpgtInstance::~SpgtInstance()
{
	if (m_hndLibrary)
	{
		FreeLibrary(m_hndLibrary);
	}
}

bool SpgtInstance::isAvailable()
{
	return m_mutLock.try_lock();
}

char* SpgtInstance::parse(char* expression, int digitlimit)
{
	char* szResult = mp_libparse(expression, digitlimit);
	m_mutLock.unlock();
	return szResult;
}