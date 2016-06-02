#include "dCacheTools.h"

#include <QLocale>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

#include <boost/filesystem.hpp>

dCacheTools::dCacheTools()
{
	m_copy = false;
	m_check = false;
	m_list = false;

	m_dir = "";
	m_base = "";
	m_output = "";
	m_type = -1;
	m_isSingleFile = false;
	m_stop = false;
}

dCacheTools::~dCacheTools()
{
	dCacheCopy->deleteLater();
}

void dCacheTools::Configure(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile)
{
	m_dir = Input;
	m_base = BaseDir;
	m_output = OutputDir;
	m_type = type;
	m_isSingleFile = isSingleFile;
}

void dCacheTools::init()
{
	dCacheCopy = new QProcess();
	dCacheCopy->setProcessChannelMode(QProcess::SeparateChannels);
}

void dCacheTools::start()
{
	this->init();
	this->run();
}

void dCacheTools::run()
{
	if(m_copy)
		this->Copy();
	if(m_list)
		this->List();
	if(m_check)
		this->Check();

	return;
}

void dCacheTools::List()
{
	emit log("INFO", "Listing called");

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
	emit log("MESSAGE", "gfal-cp");

	QLocale::setDefault(QLocale::English);
	dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);

	std::string str;

	if(m_isSingleFile)
	{
		boost::filesystem::path p(m_dir.toStdString());
		std::string filename = p.filename().string();

		str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
		str += "file:/";
		str += m_dir.toStdString();
		str += " srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
		str += m_base.toStdString();
		str += "/";
		str += m_output.toStdString();
		str += filename;
		/*
        str += " lfn:/grid/calice/";
        str += BaseDir.toStdString();
        str += "/";
        str += OutputDir.toStdString();
        str += filename;
		 */

		emit log("DEBUG", QString::fromStdString(str));

		dCacheCopy->start(QString::fromStdString(str));

		if(!dCacheCopy->waitForStarted())
		{
			emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
			return;
		}

		if(m_stop)
			this->StopCopy();
	}
	else
	{
		QString filetype;
		if(m_type == 1)
			filetype = "txt";
		if(m_type == 2)
			filetype = "slcio";
		if(m_type == 3)
			filetype = "raw";

		QDir dir(m_dir);
		dir.setFilter(QDir::Files | QDir::NoSymLinks);
		dir.setSorting(QDir::Time | QDir::Reversed);

		QFileInfoList list = dir.entryInfoList();

		for (int i = 0; i < list.size(); ++i)
		{
			QFileInfo fileInfo = list.at(i);
			emit log("DEBUG", QString("file : %1 \t Time : %2").arg(fileInfo.fileName(), (fileInfo.lastModified()).toString(Qt::ISODate)));

			std::string filename = (fileInfo.fileName()).toStdString();
			if(fileInfo.completeSuffix() != filetype && m_type != 4) continue;

			str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
			str += "file:/";
			str += m_dir.toStdString();
			str += "/";
			str += filename;
			str += " srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
			str += m_base.toStdString();
			str += "/";
			str += m_output.toStdString();
			str += filename;
			/*
            str += " lfn:/grid/calice/";
            str += BaseDir.toStdString();
            str += "/";
            str += OutputDir.toStdString();
            str += filename;
			 */

			emit log("DEBUG", QString::fromStdString(str));

			dCacheCopy->start(QString::fromStdString(str));

			if(!dCacheCopy->waitForStarted())
			{
				emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
				//return;
			}

			if(m_stop)
			{
				this->StopCopy();
				break;
			}

			dCacheCopy->waitForFinished(-1);
		}
	}

	m_copy = false;
}

void dCacheTools::StopCopy()
{
	emit log("INFO", "Waiting for last Copy");
	dCacheCopy->waitForFinished();

	if(dCacheCopy->exitCode() != 0)
	{
		emit log("ERROR", dCacheCopy->errorString());
	}

	m_stop = false;

	dCacheCopy->deleteLater();
}

void dCacheTools::Check()
{
	m_check = false;
}
