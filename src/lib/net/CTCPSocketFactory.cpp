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

#include "CTCPSocketFactory.h"
#include "CTCPSocket.h"
#include "CTCPListenSocket.h"

//
// CTCPSocketFactory
//

CTCPSocketFactory::CTCPSocketFactory(IEventQueue* events) :
	m_events(events)
{
	// do nothing
}

CTCPSocketFactory::~CTCPSocketFactory()
{
	// do nothing
}

IDataSocket*
CTCPSocketFactory::create() const
{
	return new CTCPSocket(m_events);
}

IListenSocket*
CTCPSocketFactory::createListen() const
{
	return new CTCPListenSocket(m_events);
}
