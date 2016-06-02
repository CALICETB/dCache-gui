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
	void init();

	void Copy();
	void Check();
	void List();
	void StopCopy();

	void setFlags(bool copy, bool check, bool list) {
		m_copy = copy;
		m_check = check;
		m_list = list;
	}
	void setStopFlag(bool stop) {m_stop = stop;}

	signals:
	void log(QString type, QString message);

	public slots:
	void Configure(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile);
	void start();

	private:
	QProcess *dCacheCopy;
	bool m_copy, m_check, m_list, m_isSingleFile, m_stop;
	QString m_dir, m_base, m_output;
	int m_type;
};

#endif // DCACHETOOLS_H
