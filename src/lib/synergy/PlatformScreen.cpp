/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2014 Carlo Wood
 * Copyright (C) 2012 Bolton Software Ltd.
 * Copyright (C) 2004 Chris Schoeneman
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

#include "synergy/PlatformScreen.h"
#include "synergy/App.h"
#include "synergy/ArgsBase.h"
#include "synergy/ClientApp.h"
#include "client/Client.h"
#include "base/Log.h"

CPlatformScreen::CPlatformScreen(IEventQueue* events, bool isPrimary) :
	IPlatformScreen(events),
	m_tail(0),
	m_head(0),
	m_draggingStarted(false),
	m_fakeDraggingStarted(false),
	m_isPrimary(isPrimary),
	m_isOnScreen(isPrimary),
	m_x(0), m_y(0),
	m_w(0), m_h(0),
	m_buttons(0),
	m_rcvdMouseX(0),
	m_rcvdMouseY(0),
	m_rcvdMouse_valid(false),
	m_motionEventMouseX(0),
	m_motionEventMouseY(0),
	m_relativeMouseMode(false),
	m_locked(false),
	m_relativeCount(0)
{
}

CPlatformScreen::~CPlatformScreen()
{
	// do nothing
}

void
CPlatformScreen::enter(SInt32 xAbs, SInt32 yAbs)
{
	m_isOnScreen = true;
	m_buttons = 0;
	m_motionEventMouseX = m_rcvdMouseX = xAbs;
	m_motionEventMouseY = m_rcvdMouseY = yAbs;
	m_rcvdMouse_valid = true;
	m_relativeMouseMode = false;
	m_relativeCount = 0;
	if (!m_isPrimary) {
		mouseMove(xAbs, yAbs);
	}
}

void
CPlatformScreen::updateKeyMap()
{
	getKeyState()->updateKeyMap();
}

void
CPlatformScreen::updateKeyState()
{
	getKeyState()->updateKeyState();
	updateButtons();
}

void
CPlatformScreen::setHalfDuplexMask(KeyModifierMask mask)
{
	getKeyState()->setHalfDuplexMask(mask);
}

void
CPlatformScreen::fakeKeyDown(KeyID id, KeyModifierMask mask,
				KeyButton button)
{
	getKeyState()->fakeKeyDown(id, mask, button);
}

bool
CPlatformScreen::fakeKeyRepeat(KeyID id, KeyModifierMask mask,
				SInt32 count, KeyButton button)
{
	return getKeyState()->fakeKeyRepeat(id, mask, count, button);
}

bool
CPlatformScreen::fakeKeyUp(KeyButton button)
{
	return getKeyState()->fakeKeyUp(button);
}

void
CPlatformScreen::fakeAllKeysUp()
{
	getKeyState()->fakeAllKeysUp();
}

bool
CPlatformScreen::fakeCtrlAltDel()
{
	return getKeyState()->fakeCtrlAltDel();
}

bool
CPlatformScreen::isKeyDown(KeyButton button) const
{
	return getKeyState()->isKeyDown(button);
}

KeyModifierMask
CPlatformScreen::getActiveModifiers() const
{
	return getKeyState()->getActiveModifiers();
}

KeyModifierMask
CPlatformScreen::pollActiveModifiers() const
{
	return getKeyState()->pollActiveModifiers();
}

SInt32
CPlatformScreen::pollActiveGroup() const
{
	return getKeyState()->pollActiveGroup();
}

void
CPlatformScreen::pollPressedKeys(KeyButtonSet& pressedKeys) const
{
	getKeyState()->pollPressedKeys(pressedKeys);
}

bool
CPlatformScreen::isDraggingStarted()
{
	if (CApp::instance().argsBase().m_enableDragDrop) {
		return m_draggingStarted;
	}
	return false;
}

// The functions below are only called for secondary screens.
//
// The synergy server has a notion of where the pointer is (on the screen of the client).
// It sends either absolute coordinates or relative coordinate updates to the client.
//
// The client processes relative updates as relative updates.
// It processes absolute coordinates as absolute coordinates or as relative coordinate updates.
// The latter happens when the client is in "relative mouse movement mode".
//
// The client goes to relative mouse movement mode immediately after pressing any mouse button,
// or when a motionNotify is received that differs from the notion of where the server thinks
// the mouse is; that is when we didn't send a fakeMouseMove for the coordinates received in
// the event. In the latter case, the client sends a warp message to the server in an attempt
// to synchronize the notion of the server with where the mouse really is. But in the mean
// time every motion is processed as relative, so the that the warp is not disturbed.
//
// The client goes back to absolute mouse coordinates when all mouse buttons are released
// and the last motionNotify corresponds with the last coordinates received from the server,
// which, provided we are still receiving motionNotifications, generally should be when the
// mouse stops moving for a split second, or even sooner.
//
// The effect of the above is the following:
// If the user clicks a mouse button in an application that subsequently grabs the mouse
// pointer (so no new motion notify events are received) then things will keep working
// even if that application does mouse warping. As soon as the user releases the mouse
// button, the mouse is put back where the server thinks it is for proper screen edge
// detection.
//
// If the user grabs a window and drags it, then motion notify events are received that
// correspond to the notion of the server - so the client switches back to absolute
// coordinates on the first mouse move and proper screen edge detection works.
//

void
CPlatformScreen::mouseDown(ButtonID button)
{
	// Ignore mouse button messages if we're not on this screen.
	if (!m_isOnScreen) {
	  return;
	}

	// Fake a mouse button press.
	fakeMouseButton(button, true);

	// Keep track of mouse button states for the sake of switching between absolute and relative mouse coordinates.
	if (button < 32) {
		UInt32 mask = 1U << button;
		if (!(m_buttons & mask)) {
			if (m_buttons == 0) {
				// The first mouse button was pressed.
				LOG((CLOG_DEBUG1 "CPlatformScreen::mouseDown(%d): switched to relative mouse mode!", (int)button));
				m_relativeMouseMode = true;
			}
			m_buttons |= mask;
		}
	}
}

void
CPlatformScreen::mouseUp(ButtonID button)
{
	// Ignore mouse button messages if we're not on this screen.
	if (!m_isOnScreen) {
	  return;
	}

	// Fake a mouse button release.
	fakeMouseButton(button, false);

	// Keep track of mouse button states for the sake of switching between absolute and relative mouse coordinates.
	if (button < 32) {
		UInt32 mask = 1U << button;
		if ((m_buttons & mask)) {
			m_buttons &= ~mask;
			if (m_buttons == 0) {
				// All mouse buttons were released.
				if (m_rcvdMouse_valid &&
				    m_motionEventMouseX == m_rcvdMouseX &&
				    m_motionEventMouseY == m_rcvdMouseY &&
				    m_tail == m_head) {
					LOG((CLOG_DEBUG1 "CPlatformScreen::mouseUp(%d): switched to absolute mouse mode!"));
					m_relativeMouseMode = false;
				}
			}
		}
	}
}

void
CPlatformScreen::pushMouseMove(SInt32 x, SInt32 y)
{
	static int const stack_size = sizeof(m_mouseMoveStack) / sizeof(m_mouseMoveStack[0]);
	m_mouseMoveStack[m_head].x = x;
	m_mouseMoveStack[m_head].y = y;
	m_head = (m_head + 1) % stack_size;
	if (m_tail == m_head) {
		m_tail = (m_tail + 1) % stack_size;
	}
	LOG((CLOG_DEBUG2 "CPlatformScreen::pushMouseMove(%d, %d), stack size now: %d", x, y, (m_head - m_tail + stack_size) % stack_size));
}

bool
CPlatformScreen::popMouseMove(SInt32 x, SInt32 y)
{
	static int const stack_size = sizeof(m_mouseMoveStack) / sizeof(m_mouseMoveStack[0]);
	bool found = false;
	int old_tail = m_tail;
	while(!found && m_tail != m_head) {
		if (m_mouseMoveStack[m_tail].x == x &&
		    m_mouseMoveStack[m_tail].y == y) {
			found = true;
		}
		m_tail = (m_tail + 1) % stack_size;
	}
	if (!found) {
		m_tail = old_tail;
	}
	LOG((CLOG_DEBUG2 "CPlatformScreen::popMouseMove(%d, %d) : %s, stack size now: %d", x, y, found ? "found" : "not found!", (m_head - m_tail + stack_size) % stack_size));
	return found;
}

void
CPlatformScreen::mouseMove(SInt32 x, SInt32 y)
{
	LOG((CLOG_DEBUG2 "Entering CPlatformScreen::mouseMove(%d, %d)", x, y));

	// Pass the mouse move on to the platform screen.
	if (!m_relativeMouseMode) {
		LOG((CLOG_DEBUG2 "Calling fakeMouseMove(%d, %d)", x, y));
		fakeMouseMove(x, y);
		m_relativeCount = 0;
	}
	else if (!m_rcvdMouse_valid) {
		// After the server switches back from relative mouse movements to absolute
		// ones, warp the mouse to the last position where we saw it rather than
		// believing the server.
		CClientApp& app = CClientApp::instance();
		CClient* client = app.getClientPtr();
		client->warpMouse(m_motionEventMouseX - x, m_motionEventMouseY - y);
	}
	else {
		++m_relativeCount;
		LOG((CLOG_DEBUG2 "Calling fakeMouseRelativeMove(%d, %d) (m_relativeCount = %d)", x - m_rcvdMouseX, y - m_rcvdMouseY, m_relativeCount));
		fakeMouseRelativeMove(x - m_rcvdMouseX, y - m_rcvdMouseY);
		// We get several mouseMove messages without being synced (as a result of getting
		// a motion notify that causes a warp message to sync the server), then apparently
		// we're not getting motion notify events; assume that the running application
		// grabbed the mouse and lock to this screen so the server won't think we hit
		// the edge of the screen while really we have been faking relative mouse movements
		// for a while now.
		if (m_relativeCount == 4) {
			CClientApp& app = CClientApp::instance();
			CClient* client = app.getClientPtr();
			client->lockScreen(true);
			m_locked = true;
		}
	}

	// Keep track of what we received last from the server.
	m_rcvdMouseX = x;
	m_rcvdMouseY = y;
	m_rcvdMouse_valid = true;
}

void
CPlatformScreen::mouseRelativeMove(SInt32 x, SInt32 y)
{
	LOG((CLOG_DEBUG2 "Entering CPlatformScreen::mouseRelativeMove(%d, %d)", x, y));

	fakeMouseRelativeMove(x, y);

	// The server doesn't really care anymore where mouse is now.
	m_rcvdMouse_valid = false;
}

void
CPlatformScreen::onMotionNotify(SInt32 x, SInt32 y)
{
	LOG((CLOG_DEBUG2 "Calling CPlatformScreen::onMotionNotify(%d, %d)", x, y));


	// We don't need to take any action when the mouse isn't on this screen,
	// or when we don't have a notion of serverside mouse position.
	if (!m_isOnScreen || !m_rcvdMouse_valid) {
		return;
	}

	// We received an event that the mouse is now here.
	// Is this a warp, or was it something we did ourselves?
	bool ours = popMouseMove(x, y);

	if (m_rcvdMouse_valid && x == m_rcvdMouseX && y == m_rcvdMouseY && m_tail == m_head) {
		// We caught up.
		ours = true;
		// Switch to absolute mouse mode if no mouse buttons are pressed.
		if (m_relativeMouseMode && m_buttons == 0) {
			m_relativeMouseMode = false;
			LOG((CLOG_DEBUG1 "Caught up: switched to absolute mouse mode!"));
		}
	}
	if (m_buttons == 0 && m_locked) {
		CClientApp& app = CClientApp::instance();
		CClient* client = app.getClientPtr();
		client->lockScreen(false);
		m_locked = false;
		if (m_relativeMouseMode) {
			// We're going to receive a mouseMove (after having received mouseRelativeMove calls).
			// remember where the mouse *should have been*.
			m_motionEventMouseX = x;
			m_motionEventMouseY = y;
			m_rcvdMouse_valid = false;
		}
	}
	if (m_relativeMouseMode) {
		m_tail = m_head;
		return;
	}
	if (!ours) {
		// We're in absolute mouse mode and received a motion event
		// for coordinates that we didn't send to the platform screen
		// ourselves in the recent past; this must be caused by
		// an external mouse warp.

		m_relativeMouseMode = true;
		LOG((CLOG_DEBUG1 "CPlatformScreen::onMotionNotify: detected mouse warp from %d, %d --> %d, %d ; switch to relative mouse mode!", m_motionEventMouseX, m_motionEventMouseY, x, y));

		// Inform the server about this!
		CClientApp& app = CClientApp::instance();
		CClient* client = app.getClientPtr();
		client->warpMouse(x - m_motionEventMouseX, y - m_motionEventMouseY);
	}

	// Keep track of what we received as last event.
	m_motionEventMouseX = x;
	m_motionEventMouseY = y;
}

void
CPlatformScreen::mouseWarp(SInt32 x, SInt32 y)
{
	if (!m_rcvdMouse_valid) {
		return;
	}

	m_rcvdMouseX += x;
	m_rcvdMouseY += y;

	LOG((CLOG_DEBUG1 "CPlatformScreen::mouseWarp(%d, %d), m_rcvdMouse %d, %d --> %d, %d", x, y, m_rcvdMouseX - x, m_rcvdMouseY - y, m_rcvdMouseX, m_rcvdMouseY));
	if (!m_buttons) {
		// Because we don't keep track of motion events while in relative mouse mode
		// (because that is not possible), it's can happen that more mouse warps
		// have occured in the meantime. In order to catch any remaining offset
		// we switch back to absolute mouse mode here in order to be able to detect
		// that.
		m_relativeMouseMode = false;
		LOG((CLOG_DEBUG1 "Received mouse warp and no mouse buttons are pressed: switched to absolute mouse mode!"));
	}
}

