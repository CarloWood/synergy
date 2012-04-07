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
 
#include "CArchAppUtil.h"

CArchAppUtil* CArchAppUtil::s_instance = nullptr;
 
CArchAppUtil::CArchAppUtil() :
m_app(nullptr)
{
	s_instance = this;
}

CArchAppUtil::~CArchAppUtil()
{
}

bool 
CArchAppUtil::parseArg(const int& argc, const char* const* argv, int& i)
{
	// no common platform args (yet)
	return false;
}

void
CArchAppUtil::adoptApp(IApp* app)
{
	app->setByeFunc(&exitAppStatic);
	m_app = app;
}

IApp&
CArchAppUtil::app() const
{
	assert(m_app != nullptr);
	return *m_app;
}

CArchAppUtil&
CArchAppUtil::instance()
{
	assert(s_instance != nullptr);
	return *s_instance;
}
