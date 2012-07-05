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

#pragma once

#include "CNetworkAddress.h"
#include "CTCPSocket.h"

class CIpcServerProxy;
class CIpcMessage;

//! IPC client for communication between daemon and GUI.
/*!
 * See \ref CIpcServer description.
 */
class CIpcClient {
public:
	CIpcClient();
	virtual ~CIpcClient();

	//! @name manipulators
	//@{

	//! Connects to the IPC server at localhost.
	void				connect();

	//! Sends a message to the server.
	void				send(const CIpcMessage& message);

	//@}
	//! @name accessors
	//@{

	//! Raised when the socket is connected.
	static CEvent::Type	getConnectedEvent();

	//@}

private:
	void				handleConnected(const CEvent&, void*);

private:
	CNetworkAddress		m_serverAddress;
	CTCPSocket			m_socket;
	CIpcServerProxy*	m_server;

	static CEvent::Type	s_connectedEvent;
};
