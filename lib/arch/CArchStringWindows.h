/*
 * synergy-plus -- mouse and keyboard sharing utility
 * Copyright (C) 2009 The Synergy+ Project
 * Copyright (C) 2002 Chris Schoeneman
 * 
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file COPYING that should have accompanied this file.
 * 
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CARCHSTRINGWINDOWS_H
#define CARCHSTRINGWINDOWS_H

#include "IArchString.h"

#define ARCH_STRING CArchStringWindows

//! Win32 implementation of IArchString
class CArchStringWindows : public IArchString {
public:
	CArchStringWindows();
	virtual ~CArchStringWindows();

	// IArchString overrides
	virtual int			vsnprintf(char* str,
							int size, const char* fmt, va_list ap);
	virtual int			convStringMBToWC(wchar_t*,
							const char*, UInt32 n, bool* errors);
	virtual int			convStringWCToMB(char*,
							const wchar_t*, UInt32 n, bool* errors);
	virtual EWideCharEncoding
						getWideCharEncoding();
};

#endif
