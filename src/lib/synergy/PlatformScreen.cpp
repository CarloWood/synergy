/*
 * synergy -- mouse and keyboard sharing utility
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
#include "base/Log.h"

CPlatformScreen::CPlatformScreen(IEventQueue* events, bool isPrimary) :
	IPlatformScreen(events),
	m_draggingStarted(false),
	m_fakeDraggingStarted(false),
	m_isPrimary(isPrimary),
	m_isOnScreen(isPrimary),
	m_x(0), m_y(0),
	m_w(0), m_h(0)
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

void
CPlatformScreen::mouseMove(SInt32 x, SInt32 y)
{
	LOG((CLOG_DEBUG2 "Entering CPlatformScreen::mouseMove(%d, %d)", x, y));

		fakeMouseMove(x, y);
}

void
CPlatformScreen::mouseWarp(SInt32 x, SInt32 y)
{
}

