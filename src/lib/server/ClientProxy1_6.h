/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2014 Carlo Wood
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

#include "server/ClientProxy1_5.h"

class CServer;
class IEventQueue;

//! Proxy for client implementing protocol version 1.6
class CClientProxy1_6 : public CClientProxy1_5 {
public:
	CClientProxy1_6(const CString& name, synergy::IStream* adoptedStream, CServer* server, IEventQueue* events);
	~CClientProxy1_6();

	virtual bool		parseMessage(const UInt8* code);
	virtual bool		isLockedToScreen() const { return m_isLockedToScreen; }
	virtual void		unlockScreen() { m_isLockedToScreen = false; }

	void			mouseWarp(SInt32 x, SInt32 y);	// Send
	void			mouseWarp();			// Receive
	void			lockScreen();			// Receive

private:
	bool			m_isLockedToScreen;
};
