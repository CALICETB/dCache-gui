#include "dCacheTools.h"

#include <QLocale>
#include <QDir>
#include <QDateTime>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

dCacheTools::dCacheTools()
{
	m_copy = false;
	m_check = false;
	m_list = false;

	m_dir = "";
	m_base = "";
	m_output = "";
	m_isSingleFile = false;
	m_stop = false;

	nfiles = 0;
	idxProcess = 0;
	Threaddelay = 5;
}

dCacheTools::~dCacheTools()
{
	dCacheCopy->deleteLater();
}

void dCacheTools::delay(int secs)
{
    QTime dieTime = QTime::currentTime().addSecs(secs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void dCacheTools::Configure(QString Input, QString BaseDir, QString OutputDir, bool isSingleFile)
{
	m_dir = Input;
	m_base = BaseDir;
	m_output = OutputDir;
	m_isSingleFile = isSingleFile;
}

void dCacheTools::start()
{
	this->run();
}

void dCacheTools::run()
{
	if(m_copy)
	{
		this->Copy();
	}
	if(m_list)
		this->List();
	if(m_check)
		this->Check();

	return;
}

void dCacheTools::List()
{
	emit log("INFO", QString("Listing Directory %1").arg(m_base+m_output));

	dCacheCopy = new QProcess();
	dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);

	std::string str = "/usr/bin/gfal-ls -l ";
	str += "srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
	str += (m_base+m_output).toStdString();

	dCacheCopy->start(QString::fromStdString(str));

	if(!dCacheCopy->waitForStarted())
	{
		emit log("ERROR", QString("gfal-ls %1").arg(dCacheCopy->errorString()));
		return;
	}

	dCacheCopy->waitForFinished();

	if(dCacheCopy->exitCode() != 0)
	{
		emit log("ERROR", dCacheCopy->errorString());
	}

	m_list = false;
}

void dCacheTools::Copy()
{
	emit log("INFO", "Starting Copying..");

	QLocale::setDefault(QLocale::English);

	if(m_isSingleFile)
	{
		QFileInfo file(m_dir);
		list.push_back(file);
		nfiles = list.size();
	}
	else
	{
		QDir dir(m_dir);
		dir.setFilter(QDir::Files | QDir::NoSymLinks);
		dir.setSorting(QDir::Time | QDir::Reversed);

		list = dir.entryInfoList();
		nfiles = list.size();
	}

	if(nfiles > 0)
	{
		if(m_stop)
			return;

		idxProcess = 0;

		QFileInfo fileInfo = list.at(idxProcess);

		QString filename = fileInfo.fileName();
		QDateTime fileTime = fileInfo.lastModified();
		QDateTime current = QDateTime::currentDateTime();

		emit log("DEBUG", QString("File : %1 \n "
				"Last Modification : %2 \n "
				"Current time : %3").arg(filename, fileTime.toString(Qt::LocalDate), current.toString(Qt::LocalDate)));

		if(current.toTime_t() - fileTime.toTime_t() < 350)
		{
			emit log("DEBUG", "File too young. Will be copied later");
			idxProcess++;
			this->goToNextFile();
			return;
		}

		std::string str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
		str += "file:/";
		if(m_isSingleFile)
			str += m_dir.toStdString();
		else
		{
			str += m_dir.toStdString();
			str += "/";
			str += filename.toStdString();
		}
		str += " srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
		str += m_base.toStdString();
		str += "/";
		str += m_output.toStdString();
		str += filename.toStdString();
		/*
            str += " lfn:/grid/calice/";
            str += BaseDir.toStdString();
            str += "/";
            str += OutputDir.toStdString();
            str += filename;
		 */

		dCacheCopy = new QProcess();
		dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);
		dCacheCopy->start(QString::fromStdString(str));

		if(!dCacheCopy->waitForStarted())
		{
			emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
			//return;
		}

		connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
		idxProcess++;
	}
}

void dCacheTools::Check()
{
	m_check = false;
}

void dCacheTools::finishedProcess (int exitCode, QProcess::ExitStatus exitStatus)
{
	emit log("INFO", QString("Completed with exit code %1").arg(QString::number(exitCode)));

	this->delay(Threaddelay);

	if(idxProcess < nfiles)
	{
		if(m_stop)
			return;

		QFileInfo fileInfo = list.at(idxProcess);

		QString filename = fileInfo.fileName();
		QDateTime fileTime = fileInfo.lastModified();
		QDateTime current = QDateTime::currentDateTime();

		emit log("DEBUG", QString("File : %1 \n "
				"Last Modification : %2 \n "
				"Current time : %3").arg(filename, fileTime.toString(Qt::LocalDate), current.toString(Qt::LocalDate)));

		if(current.toTime_t() - fileTime.toTime_t() < 350)
		{
			emit log("DEBUG", "File too young. Going to next file.");
			idxProcess++;
			this->goToNextFile();
			return;
		}

		std::string str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
		str += "file:/";
		if(m_isSingleFile)
			str += m_dir.toStdString();
		else
		{
			str += m_dir.toStdString();
			str += "/";
			str += filename.toStdString();
		}
		str += " srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
		str += m_base.toStdString();
		str += "/";
		str += m_output.toStdString();
		str += filename.toStdString();
		/*
    	            str += " lfn:/grid/calice/";
    	            str += BaseDir.toStdString();
    	            str += "/";
    	            str += OutputDir.toStdString();
    	            str += filename;
		 */

		/* create QProcess object */
		dCacheCopy = new QProcess();
		dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);
		dCacheCopy->start(QString::fromStdString(str));

		/* show output */
		if(!dCacheCopy->waitForStarted())
		{
			emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
			//return;
		}

		connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
		idxProcess++;
	}
	else
		this->start();
}

void dCacheTools::goToNextFile()
{
	this->delay(Threaddelay);

	if(idxProcess < nfiles)
	{
		if(m_stop)
			return;

		QFileInfo fileInfo = list.at(idxProcess);

		QString filename = fileInfo.fileName();
		QDateTime fileTime = fileInfo.lastModified();
		QDateTime current = QDateTime::currentDateTime();

		emit log("DEBUG", QString("File : %1 \n "
				"Last Modification : %2 \n "
				"Current time : %3").arg(filename, fileTime.toString(Qt::LocalDate), current.toString(Qt::LocalDate)));

		if(current.toTime_t() - fileTime.toTime_t() < 350)
		{
			emit log("DEBUG", "File too young. Going to next file.");
			idxProcess++;
			this->goToNextFile();
			return;
		}

		std::string str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
		str += "file:/";
		if(m_isSingleFile)
			str += m_dir.toStdString();
		else
		{
			str += m_dir.toStdString();
			str += "/";
			str += filename.toStdString();
		}
		str += " srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
		str += m_base.toStdString();
		str += "/";
		str += m_output.toStdString();
		str += filename.toStdString();
		/*
    	            str += " lfn:/grid/calice/";
    	            str += BaseDir.toStdString();
    	            str += "/";
    	            str += OutputDir.toStdString();
    	            str += filename;
		 */

		/* create QProcess object */
		dCacheCopy = new QProcess();
		dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);
		dCacheCopy->start(QString::fromStdString(str));

		/* show output */
		if(!dCacheCopy->waitForStarted())
		{
			emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
			//return;
		}

		connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
		idxProcess++;
	}
	else
		this->start();
}
