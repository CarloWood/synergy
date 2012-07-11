/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012 Nick Bolton
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

#include "CIpcServerProxy.h"
#include "IStream.h"
#include "TMethodEventJob.h"
#include "CLog.h"
#include "CIpcMessage.h"
#include "Ipc.h"
#include "CProtocolUtil.h"

CEvent::Type			CIpcServerProxy::s_messageReceivedEvent = CEvent::kUnknown;

CIpcServerProxy::CIpcServerProxy(synergy::IStream& stream) :
m_stream(stream)
{
	EVENTQUEUE->adoptHandler(m_stream.getInputReadyEvent(),
		stream.getEventTarget(),
		new TMethodEventJob<CIpcServerProxy>(
		this, &CIpcServerProxy::handleData));
}

CIpcServerProxy::~CIpcServerProxy()
{
	EVENTQUEUE->removeHandler(m_stream.getInputReadyEvent(),
		m_stream.getEventTarget());

	m_stream.close();
}

void
CIpcServerProxy::handleData(const CEvent&, void*)
{
	LOG((CLOG_DEBUG "start ipc handle data"));

	UInt8 code[4];
	UInt32 n = m_stream.read(code, 4);
	while (n != 0) {

		LOG((CLOG_DEBUG "ipc read: %c%c%c%c",
			code[0], code[1], code[2], code[3]));
		
		CIpcMessage* m = nullptr;
		if (memcmp(code, kIpcMsgLogLine, 4) == 0) {
			m = parseLogLine();
		}
		else if (memcmp(code, kIpcMsgShutdown, 4) == 0) {
			m = new CIpcShutdownMessage();
		}
		else {
			LOG((CLOG_ERR "invalid message"));
			disconnect();
		}
		
		// don't delete with this event; the data is passed to a new event.
		CEvent e(getMessageReceivedEvent(), this, NULL, CEvent::kDontFreeData);
		e.setDataObject(m);
		EVENTQUEUE->addEvent(e);

		n = m_stream.read(code, 4);
	}
	
	LOG((CLOG_DEBUG "finished ipc handle data"));
}

void
CIpcServerProxy::send(const CIpcMessage& message)
{
	LOG((CLOG_DEBUG "ipc write: %d", message.type()));

	switch (message.type()) {
	case kIpcHello: {
		const CIpcHelloMessage& hm = static_cast<const CIpcHelloMessage&>(message);
		CProtocolUtil::writef(&m_stream, kIpcMsgHello, hm.clientType());
		break;
	}

	case kIpcCommand: {
		const CIpcCommandMessage& cm = static_cast<const CIpcCommandMessage&>(message);
		CString command = cm.command();
		CProtocolUtil::writef(&m_stream, kIpcMsgCommand, &command);
		break;
	}

	default:
		LOG((CLOG_ERR "message not supported: %d", message.type()));
		break;
	}
}

CIpcLogLineMessage*
CIpcServerProxy::parseLogLine()
{
	CString logLine;
	CProtocolUtil::readf(&m_stream, kIpcMsgLogLine + 4, &logLine);
	
	// must be deleted by event handler.
	return new CIpcLogLineMessage(logLine);
}

void
CIpcServerProxy::disconnect()
{
	LOG((CLOG_NOTE "disconnect, closing stream"));
	m_stream.close();
}

CEvent::Type
CIpcServerProxy::getMessageReceivedEvent()
{
	return EVENTQUEUE->registerTypeOnce(
		s_messageReceivedEvent, "CIpcServerProxy::messageReceived");
}
