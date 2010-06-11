#if !defined(MAINWINDOW__H)

#define MAINWINDOW__H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QProcess>

#include "ui_MainWindowBase.h"

#include "ServerConfig.h"
#include "AppConfig.h"

class QAction;
class QMenu;
class QLineEdit;
class QGroupBox;
class QPushButton;
class QTextEdit;
class QComboBox;
class QTabWidget;
class QCheckBox;
class QRadioButton;
class QTemporaryFile;

class LogDialog;
class QSynergyApplication;

class MainWindow : public QMainWindow, public Ui::MainWindowBase
{
	Q_OBJECT

	friend class QSynergyApplication;

	public:
		enum qSynergyState
		{
			synergyDisconnected,
			synergyConnected
		};

		enum qSynergyType
		{
			synergyClient,
			synergyServer
		};

	public:
		MainWindow(QWidget* parent);
		~MainWindow();

	public:
		void setVisible(bool visible);
		int synergyType() const { return m_pGroupClient->isChecked() ? synergyClient : synergyServer; }
		int synergyState() const { return m_SynergyState; }

	protected slots:
		void on_m_pGroupClient_toggled(bool on) { m_pGroupServer->setChecked(!on); }
		void on_m_pGroupServer_toggled(bool on) { m_pGroupClient->setChecked(!on); }
		bool on_m_pButtonBrowseConfigFile_clicked();
		void on_m_pButtonConfigureServer_clicked();
		bool on_m_pActionSave_triggered();
		void on_m_pActionAbout_triggered();
		void on_m_pActionSettings_triggered();
		void on_m_pActionLogOutput_triggered();
		void synergyFinished(int exitCode, QProcess::ExitStatus);
		void iconActivated(QSystemTrayIcon::ActivationReason reason);
		void startSynergy();
		void stopSynergy();

	protected:
		QSettings& settings() { return m_Settings; }
		AppConfig& appConfig() { return m_AppConfig; }
		QProcess*& synergyProcess() { return m_pSynergy; }
		void setSynergyProcess(QProcess* p) { m_pSynergy = p; }
		ServerConfig& serverConfig() { return m_ServerConfig; }
		void initConnections();
		void createMenuBar();
		void createStatusBar();
		void createTrayIcon();
		void loadSettings();
		void saveSettings();
		void setIcon(qSynergyState state);
		void setSynergyState(qSynergyState state);
		bool checkForApp(int which, QString& app);
		bool clientArgs(QStringList& args, QString& app);
		bool serverArgs(QStringList& args, QString& app);
		void setStatus(const QString& status);
		bool detectPath(const QString& name, QString& path);

	private:
		QSettings m_Settings;
		AppConfig m_AppConfig;
		QProcess* m_pSynergy;
		int m_SynergyState;
		ServerConfig m_ServerConfig;
		QTemporaryFile* m_pTempConfigFile;
		LogDialog* m_pLogDialog;

		QSystemTrayIcon* m_pTrayIcon;
		QMenu* m_pTrayIconMenu;
};

#endif

