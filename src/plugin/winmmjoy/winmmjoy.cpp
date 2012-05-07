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

#include "winmmjoy.h"

#include <MMSystem.h>
#include <iostream>
#include <sstream>

#pragma comment(lib, "winmm.lib")

std::stringstream _logStream;
#define LOG(s) \
	_logStream.str(""); \
	_logStream << "winmmjoy: " << s << std::endl; \
	s_log( _logStream.str().c_str())

static bool s_running = true;
static void (*s_sendEvent)(const char*, void*) = NULL;
static void (*s_log)(const char*) = NULL;

extern "C" {

int
init(void (*sendEvent)(const char*, void*), void (*log)(const char*))
{
	s_sendEvent = sendEvent;
	s_log = log;
	LOG("init");
	CreateThread(NULL, 0, mainLoop, NULL, 0, NULL);
	return 0;
}

int
cleanup()
{
	LOG("cleanup");
	s_running = false;
	return 0;
}

}

DWORD WINAPI
mainLoop(void* data)
{
	const char* buttonsEvent = "IPrimaryScreen::getGameDeviceButtonsEvent";
	const char* sticksEvent = "IPrimaryScreen::getGameDeviceSticksEvent";
	const char* triggersEvent = "IPrimaryScreen::getGameDeviceTriggersEvent";

	JOYINFOEX joyInfo;
	ZeroMemory(&joyInfo, sizeof(joyInfo));
	joyInfo.dwSize = sizeof(joyInfo);
	joyInfo.dwFlags = JOY_RETURNALL;
	
	UINT index = JOYSTICKID1;
	DWORD buttons, buttonsLast = 0;

	while (s_running) {
		
		if (joyGetPosEx(index, &joyInfo) != JOYERR_NOERROR) {
			LOG("waiting for joystick");
			Sleep(1000);
			break;
		}

		buttons = joyInfo.dwButtons;

		if (buttons != buttonsLast) {
			// note: synergy buttons are 16-bit, winmm's are 32-bit.
			s_sendEvent(buttonsEvent,
				new CGameDeviceButtonInfo(index, (GameDeviceButton)joyInfo.dwButtons));
		}

		buttonsLast = buttons;
		Sleep(1);
	}
	return 0;
}
