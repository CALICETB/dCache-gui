#ifndef DCACHEMAINWINDOW_H
#define DCACHEMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "Logger.h"
#include "dCacheTools.h"
#include "ProxyTools.h"

namespace Ui {
class dCacheMainWindow;
}

class dCacheMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit dCacheMainWindow(QWidget *parent = 0);
	~dCacheMainWindow();

	void init();

	signals :
	//Signal to Logger
	void log(QString type, QString message);
	void Configure_dCacheTool(QString InputDir, QString BaseDir, QString OutputDir, int type, bool isSingleFile);

	private slots:
	void on_toolButton_clicked();
	void Configure();
	void showPassword();
	void updateProxy(QString status);
	void updateMainWindow();

	void StartCopy();
	void StopCopy();
	void ListFiles();
	void CheckCopy();

	void Close();

	private:
	Ui::dCacheMainWindow *ui;
	Logger *m_logger;
	dCacheTools *m_tools;
	ProxyTools *m_proxy;
	QTimer *timer;

	QString InputDir, BaseDir, OutputDir;
	bool isLabview, isEUDAQ, isLED, isRaw, isOther, isSingleFile;

	int timeleft, timertime, type;

};

#endif // DCACHEMAINWINDOW_H
