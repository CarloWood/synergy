/*
 * synergy-plus -- mouse and keyboard sharing utility
 * Copyright (C) 2009 The Synergy+ Project
 * Copyright (C) 2006 Chris Schoeneman
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

#ifndef CINFO_H
#define CINFO_H

#include "CString.h"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

//! Info dialog for Microsoft Windows launcher
class CInfo {
public:
	CInfo(HWND parent);
	~CInfo();

	//! @name manipulators
	//@{

	//! Run dialog
	/*!
	Display and handle the dialog until closed by the user.
	*/
	void				doModal();

	//@}
	//! @name accessors
	//@{

	//@}

private:
	void				init(HWND hwnd);

	// message handling
	BOOL				doDlgProc(HWND, UINT, WPARAM, LPARAM);
	static BOOL CALLBACK dlgProc(HWND, UINT, WPARAM, LPARAM);

private:
	static CInfo*		s_singleton;

	HWND				m_parent;
};

#endif
