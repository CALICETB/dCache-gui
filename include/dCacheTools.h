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
	void delay(int secs);
	int StripRunNumber(QString filename, int type);

	void Copy();
	void Check();
	void Check(QString srm, QString base, QString output, QString file);
	void List();
	void goToNextFile();

	void setFlags(bool copy, bool check, bool list) {
		m_copy = copy;
		m_check = check;
		m_list = list;
	}
	void setStopFlag(bool stop) {m_stop = stop;}

	int getRunNumber() {return lastrunNumber;}

	signals:
	void log(QString type, QString message);
	void started();
	void finished();

	public slots:
	void Configure(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile);
	void start();
	void finishedProcess (int exitCode, QProcess::ExitStatus exitStatus);
	void finishedCheck(int exitCode, QProcess::ExitStatus exitStatus);

	private:
	QProcess *dCacheCopy;
	bool m_copy, m_check, m_list, m_isSingleFile, m_stop, isOndCache, dry_run;
	QString m_dir, m_base, m_output, Checkfilename;
	int idxProcess, nfiles, Threaddelay, lastrunNumber, m_type, indexfile;
	QFileInfoList list;
};

#endif // DCACHETOOLS_H
