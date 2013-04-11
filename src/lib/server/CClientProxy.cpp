/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Bolton Software Ltd.
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

#include "CClientProxy.h"
#include "CProtocolUtil.h"
#include "IStream.h"
#include "CLog.h"
#include "CEventQueue.h"

//
// CClientProxy
//

CEvent::Type			CClientProxy::s_readyEvent           = CEvent::kUnknown;
CEvent::Type			CClientProxy::s_disconnectedEvent    = CEvent::kUnknown;
CEvent::Type			CClientProxy::s_clipboardChangedEvent= CEvent::kUnknown;
CEvent::Type			CClientProxy::s_gameDeviceTimingRecvEvent= CEvent::kUnknown;

CClientProxy::CClientProxy(const CString& name, synergy::IStream* stream) :
	CBaseClientProxy(name),
	m_stream(stream)
{
}

CClientProxy::~CClientProxy()
{
	delete m_stream;
}

void
CClientProxy::close(const char* msg)
{
	LOG((CLOG_DEBUG1 "send close \"%s\" to \"%s\"", msg, getName().c_str()));
	CProtocolUtil::writef(getStream(), msg);

	// force the close to be sent before we return
	getStream()->flush();
}

synergy::IStream*
CClientProxy::getStream() const
{
	return m_stream;
}

CEvent::Type
CClientProxy::getReadyEvent()
{
	LOG((CLOG_DEBUG4 "getReadyEvent"));
	return EVENTQUEUE->registerTypeOnce(s_readyEvent,
							"CClientProxy::ready");
}

CEvent::Type
CClientProxy::getDisconnectedEvent()
{
	LOG((CLOG_DEBUG4 "getDisconnectedEvent"));
	return EVENTQUEUE->registerTypeOnce(s_disconnectedEvent,
							"CClientProxy::disconnected");
}

CEvent::Type
CClientProxy::getClipboardChangedEvent()
{
	LOG((CLOG_DEBUG4 "getClipboardChangedEvent"));
	return EVENTQUEUE->registerTypeOnce(s_clipboardChangedEvent,
							"CClientProxy::clipboardChanged");
}

CEvent::Type
CClientProxy::getGameDeviceTimingRespEvent()
{
	return EVENTQUEUE->registerTypeOnce(s_gameDeviceTimingRecvEvent,
							"CClientProxy::gameDeviceTimingRecv");
}

void*
CClientProxy::getEventTarget() const
{
	return static_cast<IScreen*>(const_cast<CClientProxy*>(this));
}
