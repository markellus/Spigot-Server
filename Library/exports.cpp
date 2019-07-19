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

#include <string>
using std::string;

#include "../../spigot/spigot.h"
#include "../../spigot/funcs.h"
#include "../../spigot/expr.h"
#include "../../spigot/error.h"
#include "../../spigot/io.h"
#include "../../spigot/baseout.h"

class NullFileAccessContextX : public FileAccessContext {
	virtual unique_ptr<FileOpener> file_opener(const string&) override;
	virtual unique_ptr<FileOpener> fd_opener(int) override;
	virtual unique_ptr<FileOpener> stdin_opener() override;
};

unique_ptr<FileOpener> NullFileAccessContextX::file_opener(const string&)
{
	throw io_error("file keywords not permitted in this context");
}

unique_ptr<FileOpener> NullFileAccessContextX::fd_opener(int)
{
	throw io_error("fd keywords not permitted in this context");
}

unique_ptr<FileOpener> NullFileAccessContextX::stdin_opener()
{
	throw io_error("stdin keywords not permitted in this context");
}

char* parse(char* expression, int digitlimit)
{
	const auto fac = std::make_unique<NullFileAccessContextX>();
	auto sp = expr_parse(expression, nullptr, *fac);
	unique_ptr<OutputGenerator> og = base_format (std::move(sp), 10, false, digitlimit != -1, digitlimit, ROUND_TOWARD_ZERO, 0);
	std::string out, outi;
	while (out.size() < digitlimit)
	{
		if(og->get_definite_output(outi) && !outi.empty())
		{
			out = out.append(outi);
		}
		else
		{
			break;
		}
	}
	char* s_szBuffer = new char[out.size() + 1];
	memcpy(s_szBuffer, out.c_str(), out.size());
	s_szBuffer[out.size()] = '\0';
	return s_szBuffer;
}