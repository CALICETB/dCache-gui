#ifndef DCACHETOOLS_H
#define DCACHETOOLS_H

#include <QString>
#include <QThread>
#include <QProcess>

class dCacheTools : public QThread
{
	Q_OBJECT
public:
	dCacheTools();
	~dCacheTools();

	void run();

	signals:
	void log(QString type, QString message);

	public slots:
	void Copy(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile);
	void StopCopy();
	void DoList(QString dir);

	private:
	QProcess *dCacheCopy;
};

#endif // DCACHETOOLS_H
