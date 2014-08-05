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

#pragma once

#include "synergy/IPlatformScreen.h"
#include "synergy/DragInformation.h"
#include "common/stdexcept.h"

//! Base screen implementation
/*!
This screen implementation is the superclass of all other screen
implementations.  It implements a handful of methods and requires
subclasses to implement the rest.
*/
class CPlatformScreen : public IPlatformScreen {
public:
	CPlatformScreen(IEventQueue* events, bool isPrimary);
	virtual ~CPlatformScreen();

	// IScreen overrides
	virtual void*		getEventTarget() const = 0;
	virtual bool		getClipboard(ClipboardID id, IClipboard*) const = 0;
	virtual void		getShape(SInt32& x, SInt32& y,
							SInt32& width, SInt32& height) const = 0;
	virtual void		getCursorPos(SInt32& x, SInt32& y) const = 0;

	// IPrimaryScreen overrides
	virtual void		reconfigure(UInt32 activeSides) = 0;
	virtual void		warpCursor(SInt32 x, SInt32 y) = 0;
	virtual UInt32		registerHotKey(KeyID key,
							KeyModifierMask mask) = 0;
	virtual void		unregisterHotKey(UInt32 id) = 0;
	virtual void		fakeInputBegin() = 0;
	virtual void		fakeInputEnd() = 0;
	virtual SInt32		getJumpZoneSize() const = 0;
	virtual bool		isAnyMouseButtonDown(UInt32& buttonID) const = 0;
	virtual void		getCursorCenter(SInt32& x, SInt32& y) const = 0;

	// ISecondaryScreen overrides
	virtual void		fakeMouseButton(ButtonID id, bool press) = 0;
	virtual void		fakeMouseMove(SInt32 x, SInt32 y) = 0;
	virtual void		fakeMouseRelativeMove(SInt32 dx, SInt32 dy) const = 0;
	virtual void		fakeMouseWheel(SInt32 xDelta, SInt32 yDelta) const = 0;
	virtual void		mouseWarp(SInt16 x, SInt16 y);

	// IKeyState overrides
	virtual void		updateKeyMap();
	virtual void		updateKeyState();
	virtual void		setHalfDuplexMask(KeyModifierMask);
	virtual void		fakeKeyDown(KeyID id, KeyModifierMask mask,
							KeyButton button);
	virtual bool		fakeKeyRepeat(KeyID id, KeyModifierMask mask,
							SInt32 count, KeyButton button);
	virtual bool		fakeKeyUp(KeyButton button);
	virtual void		fakeAllKeysUp();
	virtual bool		fakeCtrlAltDel();
	virtual bool		isKeyDown(KeyButton) const;
	virtual KeyModifierMask
						getActiveModifiers() const;
	virtual KeyModifierMask
						pollActiveModifiers() const;
	virtual SInt32		pollActiveGroup() const;
	virtual void		pollPressedKeys(KeyButtonSet& pressedKeys) const;

	virtual void		setDraggingStarted(bool started) { m_draggingStarted = started; }
	virtual bool		isDraggingStarted();
	virtual bool		isFakeDraggingStarted() { return m_fakeDraggingStarted; }
	virtual CString&	getDraggingFilename() { return m_draggingFilename; }
	virtual void		clearDraggingFilename() { }

	// IPlatformScreen overrides
	virtual void		enable() = 0;
	virtual void		disable() = 0;
	virtual void		enter(SInt32 xAbs, SInt32 yAbs) = 0;
	virtual bool		leave() = 0;
	virtual bool		setClipboard(ClipboardID, const IClipboard*) = 0;
	virtual void		checkClipboards() = 0;
	virtual void		openScreensaver(bool notify) = 0;
	virtual void		closeScreensaver() = 0;
	virtual void		screensaver(bool activate) = 0;
	virtual void		resetOptions() = 0;
	virtual void		setOptions(const COptionsList& options) = 0;
	virtual void		setSequenceNumber(UInt32) = 0;
	virtual bool		isPrimary() const = 0;
	
	virtual void		fakeDraggingFiles(CDragFileList fileList) { throw std::runtime_error("fakeDraggingFiles not implemented"); }
	virtual const CString&
						getDropTarget() const { throw std::runtime_error("getDropTarget not implemented"); }

protected:
	//! Update mouse buttons
	/*!
	Subclasses must implement this method to update their internal mouse
	button mapping and, if desired, state tracking.
	*/
	virtual void		updateButtons() = 0;

	//! Get the key state
	/*!
	Subclasses must implement this method to return the platform specific
	key state object that each subclass must have.
	*/
	virtual IKeyState*	getKeyState() const = 0;

	// IPlatformScreen overrides
	virtual void		handleSystemEvent(const CEvent& event, void*) = 0;
	virtual void		mouseDown(ButtonID button);
	virtual void		mouseUp(ButtonID button);
	virtual void 		mouseMove(SInt32 x, SInt32 y);		// Called when mouse update is received from the server.
	virtual void 		mouseRelativeMove(SInt32 x, SInt32 y);	// Called when a relative mouse update is received from the server.
	virtual void		onMotionNotify(SInt32 x, SInt32 y);	// Called when an event is received that the real mouse moved.

	// Manipulators.
	void			pushMouseMove(SInt32 x, SInt32 y);
	bool			popMouseMove(SInt32 x, SInt32 y);

private:
	struct { SInt32 x; SInt32 y; }	m_mouseMoveStack[6];
	int				m_tail;
	int				m_head;

protected:
	CString				m_draggingFilename;
	bool				m_draggingStarted;
	bool				m_fakeDraggingStarted;

	bool				m_isPrimary;		// True if screen is being used as a primary screen, false otherwise.
	bool				m_isOnScreen;		// true if mouse has entered the screen
	SInt32				m_x, m_y;		// Screen shape.
	SInt32				m_w, m_h;

	// The below is used to deal with mouse warps.
	UInt32				m_buttons;		// Map of mouse button states. Index is 0 is mouse button 1 etc.
	SInt32				m_rcvdMouseX;		// Last mouse position received from the server.
	SInt32				m_rcvdMouseY;
	bool				m_rcvdMouse_valid;	// True when the last mouse move message received from the server was absolute.
	SInt32				m_motionEventMouseX;	// The last known real mouse position.
	SInt32				m_motionEventMouseY;
	bool				m_relativeMouseMode;	// When true, we only pass relative mouse movements to the screen,
								// because we temporarily might have lost track of where the mouse
								// really is.
	bool				m_locked;		// True when the DLCK message to the server was a lock request.
	int				m_relativeCount;	// Number of received mouseMove messages from the server while in m_relativeMouseMode.
};
