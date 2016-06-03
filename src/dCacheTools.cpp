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
	m_type = -1;

	nfiles = 0;
	idxProcess = 0;
	lastrunNumber = 0;
	Threaddelay = 5;
	indexfile = 0;

	isOndCache = false;
	Checkfilename = "";
	dry_run = false;
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

int dCacheTools::StripRunNumber(QString filename, int type)
{
	int runNumber = 0;

	int index = filename.indexOf("Run", 0, Qt::CaseSensitive);
	if(index != -1)
	{
		if(type == 1)
		{
			QStringRef subString(&filename, index+4+3, 5);
			runNumber = subString.toInt();
		}
		if(type == 2)
		{
			QStringRef subString(&filename, index+4, 5);
			runNumber = subString.toInt();
		}
		if(type == 3)
		{
			QStringRef subString(&filename, index+4, 5);
			runNumber = subString.toInt();
		}
		else
			runNumber = 0;
	}
	else
	{
		int index = filename.indexOf("run", 0, Qt::CaseSensitive);

		if(type == 3)
		{
			QStringRef subString(&filename, index+3, 5);
			runNumber = subString.toInt();
		}
		else
			runNumber = 0;
	}

	emit log("DEBUG", QString("Striped Run %1 - index %2").arg(QString::number(runNumber), QString::number(index)));

	return runNumber;
}

void dCacheTools::Configure(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile)
{
	m_dir = Input;
	m_base = BaseDir;
	m_output = OutputDir;
	m_type = type;
	m_isSingleFile = isSingleFile;
}

void dCacheTools::start()
{
	this->run();
	emit started();
}

void dCacheTools::run()
{
	if(m_copy)
	{
		if(!m_stop)
			this->Copy();
		else
		{
			emit finished();
			return;
		}
	}
	if(m_list)
	{
		this->List();

		emit finished();
		return;
	}
	if(m_check)
	{
		this->Check();

		emit finished();
		return;
	}
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
}

void dCacheTools::Copy()
{
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

		emit log("INFO", "Starting Copying..");

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

		std::string str;
		if(dry_run)
			str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
		else
			str = "/usr/bin/gfal-copy -n 5 -t 6000 ";

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

		this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);
		lastrunNumber = StripRunNumber(filename, m_type);
		Checkfilename = filename;

		this->delay(Threaddelay);

		if(!isOndCache)
		{
			dCacheCopy = new QProcess();
			dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);
			dCacheCopy->start(QString::fromStdString(str));

			if(!dCacheCopy->waitForStarted())
			{
				emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
				return;
			}

			connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
			idxProcess++;
		}
		else
		{
			emit log("DEBUG", QString("File %1 is already on dCache").arg(filename));
			idxProcess++;
			this->goToNextFile();
			return;
		}
	}
}

void dCacheTools::Check()
{
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
		indexfile = 0;
		QFileInfo fileInfo = list.at(indexfile);
		QString filename = fileInfo.fileName();

		Checkfilename = filename;

		this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);
	}
}

void dCacheTools::Check(QString srm, QString base, QString output, QString file)
{
	dCacheCopy = new QProcess();
	dCacheCopy->setProcessChannelMode(QProcess::SeparateChannels);

	std::string str = "/usr/bin/gfal-ls -l ";
	str += srm.toStdString();
	str += base.toStdString();
	str += "/";
	str += output.toStdString();
	str += file.toStdString();

	dCacheCopy->start(QString::fromStdString(str));

	if(!dCacheCopy->waitForStarted())
	{
		emit log("ERROR", QString("gfal-ls %1").arg(dCacheCopy->errorString()));
	}

	connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedCheck(int, QProcess::ExitStatus)));
	indexfile++;
}

void dCacheTools::finishedCheck(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(m_check)
	{
		if(exitCode != 0)
			emit log("WARNING", QString("File %1 is not on dCache").arg(Checkfilename));
		else
			emit log("INFO", QString("File %1 is on dCache").arg(Checkfilename));

		if(indexfile < nfiles)
		{
			QFileInfo fileInfo = list.at(indexfile);
			QString filename = fileInfo.fileName();

			Checkfilename = filename;

			this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);
		}
	}

	if(m_copy)
	{
		if(exitCode != 0)
		{
			emit log("WARNING", QString("File %1 is not on dCache").arg(Checkfilename));
			isOndCache = false;
		}
		else
			isOndCache = true;
	}
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

		std::string str;
		if(dry_run)
			str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
		else
			str = "/usr/bin/gfal-copy -n 5 -t 6000 ";

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

		this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);
		lastrunNumber = StripRunNumber(filename, m_type);
		Checkfilename = filename;

		this->delay(Threaddelay);

		if(!isOndCache)
		{
			dCacheCopy = new QProcess();
			dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);
			dCacheCopy->start(QString::fromStdString(str));

			/* show output */
			if(!dCacheCopy->waitForStarted())
			{
				emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
				return;
			}

			connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
			idxProcess++;
		}
		else
		{
			emit log("DEBUG", QString("File %1 is already on dCache").arg(filename));
			idxProcess++;
			this->goToNextFile();
			return;
		}
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

		std::string str;
		if(dry_run)
			str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
		else
			str = "/usr/bin/gfal-copy -n 5 -t 6000 ";

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

		this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);
		lastrunNumber = StripRunNumber(filename, m_type);
		Checkfilename = filename;

		this->delay(Threaddelay);

		if(!isOndCache)
		{
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
		{
			emit log("DEBUG", QString("File %1 is already on dCache").arg(filename));
			idxProcess++;
			this->goToNextFile();
			return;
		}
	}
	else
		this->start();
}
