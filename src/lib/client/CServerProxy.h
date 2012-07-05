/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2002 Chris Schoeneman, Nick Bolton, Sorin Sbarnea
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

#ifndef CSERVERPROXY_H
#define CSERVERPROXY_H

#include "ClipboardTypes.h"
#include "KeyTypes.h"
#include "CEvent.h"
#include "GameDeviceTypes.h"

class CClient;
class CClientInfo;
class CEventQueueTimer;
class IClipboard;
namespace synergy { class IStream; }
class IEventQueue;

//! Proxy for server
/*!
This class acts a proxy for the server, converting calls into messages
to the server and messages from the server to calls on the client.
*/
class CServerProxy {
public:
	/*!
	Process messages from the server on \p stream and forward to
	\p client.
	*/
	CServerProxy(CClient* client, synergy::IStream* stream, IEventQueue& eventQueue);
	~CServerProxy();

	//! @name manipulators
	//@{

	void				onInfoChanged();
	bool				onGrabClipboard(ClipboardID);
	void				onClipboardChanged(ClipboardID, const IClipboard*);
	void				onGameDeviceTimingResp(UInt16 freq);
	void				onGameDeviceFeedback(GameDeviceID id, UInt16 m1, UInt16 m2);

	//@}

protected:
	enum EResult { kOkay, kUnknown, kDisconnect };
	EResult				parseHandshakeMessage(const UInt8* code);
	EResult				parseMessage(const UInt8* code);

private:
	// if compressing mouse motion then send the last motion now
	void				flushCompressedMouse();

	void				sendInfo(const CClientInfo&);

	void				resetKeepAliveAlarm();
	void				setKeepAliveRate(double);

	// modifier key translation
	KeyID				translateKey(KeyID) const;
	KeyModifierMask		translateModifierMask(KeyModifierMask) const;

	// event handlers
	void				handleData(const CEvent&, void*);
	void				handleKeepAliveAlarm(const CEvent&, void*);

	// message handlers
	void				enter();
	void				leave();
	void				setClipboard();
	void				grabClipboard();
	void				keyDown();
	void				keyRepeat();
	void				keyUp();
	void				mouseDown();
	void				mouseUp();
	void				mouseMove();
	void				mouseRelativeMove();
	void				mouseWheel();
	void				gameDeviceButtons();
	void				gameDeviceSticks();
	void				gameDeviceTriggers();
	void				gameDeviceTimingReq();
	void				screensaver();
	void				resetOptions();
	void				setOptions();
	void				queryInfo();
	void				infoAcknowledgment();

private:
	typedef EResult (CServerProxy::*MessageParser)(const UInt8*);

	CClient*			m_client;
	synergy::IStream*			m_stream;

	UInt32				m_seqNum;

	bool				m_compressMouse;
	bool				m_compressMouseRelative;
	SInt32				m_xMouse, m_yMouse;
	SInt32				m_dxMouse, m_dyMouse;

	bool				m_ignoreMouse;

	KeyModifierID		m_modifierTranslationTable[kKeyModifierIDLast];

	double				m_keepAliveAlarm;
	CEventQueueTimer*	m_keepAliveAlarmTimer;

	MessageParser		m_parser;
	IEventQueue&		m_eventQueue;
};

#endif
