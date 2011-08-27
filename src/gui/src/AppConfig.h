/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2008 Volker Lanz (vl@fidra.de)
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

#if !defined(APPCONFIG_H)

#define APPCONFIG_H

#include <QString>

class QSettings;
class SettingsDialog;

class AppConfig
{
	friend class SettingsDialog;
	friend class MainWindow;

	public:
		AppConfig(QSettings* settings);
		~AppConfig();

	public:
		bool autoConnect() const { return m_AutoConnect; }
		const QString& synergyc() const { return m_Synergyc; }
		const QString& synergys() const { return m_Synergys; }
		const QString& screenName() const { return m_ScreenName; }
		int port() const { return m_Port; }
		const QString& interface() const { return m_Interface; }
		int logLevel() const { return m_LogLevel; }
		bool autoDetectPaths() const { return m_AutoDetectPaths; }
		bool logToFile() const { return m_LogToFile; }
		const QString& logFilename() const { return m_LogFilename; }
		QString logLevelText() const;
                bool gameDevice() const { return m_GameDevice; }

		QString synergysName() const { return m_SynergysName; }
		QString synergycName() const { return m_SynergycName; }
		QString synergyProgramDir() const { return m_SynergyProgramDir; }
		QString synergyLogDir();

		bool detectPath(const QString& name, QString& path);
		void persistLogDir();

	protected:
		QSettings& settings() { return *m_pSettings; }
		void setAutoConnect(bool b) { m_AutoConnect = b; }
		void setSynergyc(const QString& s) { m_Synergyc = s; }
		void setSynergys(const QString& s) { m_Synergys = s; }
		void setScreenName(const QString& s) { m_ScreenName = s; }
		void setPort(int i) { m_Port = i; }
		void setInterface(const QString& s) { m_Interface = s; }
		void setLogLevel(int i) { m_LogLevel = i; }
		void setAutoDetectPaths(bool b) { m_AutoDetectPaths = b; }
                void setLogToFile(bool b) { m_LogToFile = b; }
                void setLogFilename(const QString& s) { m_LogFilename = s; }
                void setGameDevice(bool b) { m_GameDevice = b; }

		void loadSettings();
		void saveSettings();

	private:
		QSettings* m_pSettings;
		bool m_AutoConnect;
		QString m_Synergyc;
		QString m_Synergys;
		QString m_ScreenName;
		int m_Port;
		QString m_Interface;
		int m_LogLevel;
		bool m_AutoDetectPaths;
		bool m_LogToFile;
		QString m_LogFilename;
                bool m_GameDevice;

		static const char m_SynergysName[];
		static const char m_SynergycName[];
		static const char m_SynergyProgramDir[];
		static const char m_SynergyLogDir[];
};

#endif
