#ifndef DCACHETOOLS_H
#define DCACHETOOLS_H

#include <QString>
#include <QThread>
#include <QProcess>
#include <QFileInfoList>

class dCacheTools : public QThread
{
	Q_OBJECT
public:
	dCacheTools();
	~dCacheTools();

	void run();
	void init();

	void Copy();
	void Check();
	void List();
	void goToNextFile();

	void setFlags(bool copy, bool check, bool list) {
		m_copy = copy;
		m_check = check;
		m_list = list;
	}
	void setStopFlag(bool stop) {m_stop = stop;}

	signals:
	void log(QString type, QString message);
	void CopyFinished();

	public slots:
	void Configure(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile);
	void start();
	void finishedProcess (int exitCode, QProcess::ExitStatus exitStatus);

	private:
	QProcess *dCacheCopy;
	bool m_copy, m_check, m_list, m_isSingleFile, m_stop;
	QString m_dir, m_base, m_output;
	int m_type, idxProcess, nfiles;
	QFileInfoList list;
};

#endif // DCACHETOOLS_H
