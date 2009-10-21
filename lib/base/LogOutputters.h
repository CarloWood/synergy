/*
 * synergy -- mouse and keyboard sharing utility
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
 */

#ifndef LOGOUTPUTTERS_H
#define LOGOUTPUTTERS_H

#include "BasicTypes.h"
#include "ILogOutputter.h"
#include "CString.h"
#include "stddeque.h"

#include <fstream>
//! Stop traversing log chain outputter
/*!
This outputter performs no output and returns false from \c write(),
causing the logger to stop traversing the outputter chain.  Insert
this to prevent already inserted outputters from writing.
*/
class CStopLogOutputter : public ILogOutputter {
public:
	CStopLogOutputter();
	virtual ~CStopLogOutputter();

	// ILogOutputter overrides
	virtual void		open(const char* title);
	virtual void		close();
	virtual void		show(bool showIfEmpty);
	virtual bool		write(ELevel level, const char* message);
	virtual const char*	getNewline() const;
};

//! Write log to console
/*!
This outputter writes output to the console.  The level for each
message is ignored.
*/
class CConsoleLogOutputter : public ILogOutputter {
public:
	CConsoleLogOutputter();
	virtual ~CConsoleLogOutputter();

	// ILogOutputter overrides
	virtual void		open(const char* title);
	virtual void		close();
	virtual void		show(bool showIfEmpty);
	virtual bool		write(ELevel level, const char* message);
	virtual const char*	getNewline() const;
};

//! Write log to file
/*!
This outputter writes output to the file.  The level for each
message is ignored.
*/

class CFileLogOutputter : public ILogOutputter {
public:
	CFileLogOutputter(const char* logFile);
	virtual ~CFileLogOutputter();

	// ILogOutputter overrides
	virtual void		open(const char* title);
	virtual void		close();
	virtual void		show(bool showIfEmpty);
	virtual bool		write(ELevel level, const char* message);
	virtual const char*	getNewline() const;
private:
	std::ofstream		m_handle;
};

//! Write log to system log
/*!
This outputter writes output to the system log.
*/
class CSystemLogOutputter : public ILogOutputter {
public:
	CSystemLogOutputter();
	virtual ~CSystemLogOutputter();

	// ILogOutputter overrides
	virtual void		open(const char* title);
	virtual void		close();
	virtual void		show(bool showIfEmpty);
	virtual bool		write(ELevel level, const char* message);
	virtual const char*	getNewline() const;
};

//! Write log to system log only
/*!
Creating an object of this type inserts a CStopLogOutputter followed
by a CSystemLogOutputter into CLog.  The destructor removes those
outputters.  Add one of these to any scope that needs to write to
the system log (only) and restore the old outputters when exiting
the scope.
*/
class CSystemLogger {
public:
	CSystemLogger(const char* title, bool blockConsole);
	~CSystemLogger();

private:
	ILogOutputter*		m_syslog;
	ILogOutputter*		m_stop;
};

//! Save log history
/*!
This outputter records the last N log messages.
*/
class CBufferedLogOutputter : public ILogOutputter {
private:
	typedef std::deque<CString> CBuffer;

public:
	typedef CBuffer::const_iterator const_iterator;

	CBufferedLogOutputter(UInt32 maxBufferSize);
	virtual ~CBufferedLogOutputter();

	//! @name accessors
	//@{

	//! Get start of buffer
	const_iterator		begin() const;

	//! Get end of buffer
	const_iterator		end() const;

	//@}

	// ILogOutputter overrides
	virtual void		open(const char* title);
	virtual void		close();
	virtual void		show(bool showIfEmpty);
	virtual bool		write(ELevel level, const char* message);
	virtual const char*	getNewline() const;

private:
	UInt32				m_maxBufferSize;
	CBuffer				m_buffer;
};

#endif
