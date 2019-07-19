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

#include <list>
#include "SpgtInstance.h"
#include "exports.h"

bool s_bCleanupRegistered = false;
mutex* s_mutInstances = new mutex();
list<SpgtInstance*>* s_listInstances = new list<SpgtInstance*>();

char* parse(char* expression, int digitlimit)
{
	s_mutInstances->lock();

	if (!s_bCleanupRegistered)
	{
		atexit(cleanup);
		s_bCleanupRegistered = true;
	}

	for(SpgtInstance* instance : *s_listInstances)
	{
		if(instance->isAvailable())
		{
			s_mutInstances->unlock();
			return instance->parse(expression, digitlimit);
		}
	}

	auto* instance = new SpgtInstance();

	if (instance->isAvailable())
	{
		s_listInstances->push_back(instance);
		s_mutInstances->unlock();
		return instance->parse(expression, digitlimit);
	}

	s_mutInstances->unlock();
	return nullptr;
}

void cleanup()
{
	s_mutInstances->lock();
	for (SpgtInstance* instance : *s_listInstances)
	{
		delete instance;
	}
	delete s_listInstances;
	s_mutInstances->unlock();
}