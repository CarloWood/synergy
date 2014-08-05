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

#include "server/ClientProxy1_6.h"

#include "server/Server.h"
#include "synergy/ProtocolUtil.h"
#include "io/IStream.h"
#include "base/Log.h"

//
// CClientProxy1_6
//

CClientProxy1_6::CClientProxy1_6(const CString& name, synergy::IStream* stream, CServer* server, IEventQueue* events) :
	CClientProxy1_5(name, stream, server, events),
	m_isLockedToScreen(false)
{
}

CClientProxy1_6::~CClientProxy1_6()
{
}

void
CClientProxy1_6::mouseWarp(SInt16 x, SInt16 y)
{
	LOG((CLOG_DEBUG1 "CClientProxy1_6::mouseWarp(%d, %d): sending mouse warp", x, y));
	CProtocolUtil::writef(getStream(), kMsgDMouseWarp, x, y);
}

bool
CClientProxy1_6::parseMessage(const UInt8* code)
{
	if (memcmp(code, kMsgDMouseWarp, 4) == 0) {
		mouseWarp();
	}
	else if (memcmp(code, kMsgDLockScreen, 4) == 0) {
		lockScreen();
	}
	else {
		return CClientProxy1_5::parseMessage(code);
	}

	return true;
}

void
CClientProxy1_6::mouseWarp()
{
	SInt16 x, y;
	CProtocolUtil::readf(getStream(), kMsgDMouseWarp + 4, &x, &y);
	LOG((CLOG_DEBUG1 "CClientProxy1_6::mouseWarp(): received mouse warp %d, %d", x, y));

	if (m_server->mouseWarp(this, x, y))
	{
		// Send it right back.
		mouseWarp(x, y);
	}
}

void
CClientProxy1_6::lockScreen()
{
	SInt8 lock;
	CProtocolUtil::readf(getStream(), kMsgDLockScreen + 4, &lock);
	LOG((CLOG_DEBUG1 "CClientProxy1_6::lockScreen(): received lock screen %d", lock));

	m_isLockedToScreen = lock ? true : false;

	if (!m_isLockedToScreen && m_server->mouseWarp(this, 0, 0)) {
		m_server->sendMouseMove();
	}
}

