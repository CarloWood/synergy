/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2013 Bolton Software Ltd.
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

#pragma once

#include "common/stdvector.h"
#include "base/String.h"
#include "base/EventTypes.h"

typedef std::vector<CString> CDragFileList;

class CDragInformation {
public:
	CDragInformation() { }

	~CDragInformation() { }

	static void			parseDragInfo(CDragFileList& dragFileList, UInt32 fileNum, CString data);
	static CString		getDragFileExtension(CString fileName);
};
