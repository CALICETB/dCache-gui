/**
 * @file
 * @author  Eldwan Brianne <eldwan.brianne@desy.de>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 *
 */

#include "dCacheTools.h"

//-- Qt header
#include <QLocale>
#include <QDir>
#include <QDateTime>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

//--------------------------------------------------------------------------------------------------------------

dCacheTools::dCacheTools()
{
	/**
	 * Initialization of the members
	 */

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
	Threaddelay = 5;//Delay time of 5 secs
	indexfile = 0;

	isOndCache = false;
	Checkfilename = "";
	dry_run = false;//Set to true for dry-run
}

//--------------------------------------------------------------------------------------------------------------

dCacheTools::~dCacheTools()
{
	//Cleaning
	dCacheCopy->deleteLater();
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::delay(int secs)
{
	/**
	 * Delays the QThread without deadlocking the GUI
	 * by X secs
	 */

	QTime dieTime = QTime::currentTime().addSecs(secs);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//--------------------------------------------------------------------------------------------------------------

int dCacheTools::StripRunNumber(QString filename, int type)
{
	/**
	 * Get Run number from filename
	 */

	int runNumber = 0;

	int index = filename.indexOf("Run", 0, Qt::CaseSensitive);

	//Case index found
	if(index != -1)
	{
		if(type == 1)//Text files
		{
			QStringRef subString(&filename, index+4+3, 5);
			runNumber = (subString.toString()).toInt();
		}
		if(type == 2)//slcio files
		{
			QStringRef subString(&filename, index+4, 5);
			runNumber = (subString.toString()).toInt();
		}
		if(type == 3)//raw binary files
		{
			QStringRef subString(&filename, index+4, 5);
			runNumber = (subString.toString()).toInt();
		}
		else
			runNumber = 0;
	}
	//Case bif files
	else
	{
		int index = filename.indexOf("run", 0, Qt::CaseSensitive);

		if(type == 3)//raw binary files
		{
			QStringRef subString(&filename, index+3, 5);
			runNumber = (subString.toString()).toInt();
		}
		else
			runNumber = 0;
	}

	emit log("DEBUG", QString("Striped Run %1 - index %2").arg(QString::number(runNumber), QString::number(index)));//Logging

	return runNumber;
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::Configure(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile)
{
	/**
	 * Configure the parameters
	 */

	m_dir = Input;
	m_base = BaseDir;
	m_output = OutputDir;
	m_type = type;
	m_isSingleFile = isSingleFile;
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::start()
{
	/*
	 * Start the QThread
	 */

	emit started();
	this->run();
	emit finished();

	return;//end of the QThread
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::run()
{
	/*
	 * Overloaded run method
	 * Calls the Copy, Check or Listing
	 */

	if(m_copy)
	{
		this->Copy();
		return;
	}

	if(m_list)
	{
		this->List();
		return;
	}

	if(m_check)
	{
		this->Check();
		return;
	}
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::List()
{
	/*
	 * List Method
	 * List all the files in Base directory + Output directory
	 */

	emit log("INFO", QString("Listing Directory %1").arg(m_base+m_output));//Logging

	dCacheCopy = new QProcess();
	dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);//Prints stdout on console

	/*
	 * GFAL-LS command
	 */
	std::string str = "/usr/bin/gfal-ls -l ";
	str += "srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
	str += (m_base+m_output).toStdString();

	//Start QProcess
	dCacheCopy->start(QString::fromStdString(str));

	//Wait for the start of QProcess
	if(!dCacheCopy->waitForStarted())
	{
		emit log("ERROR", QString("gfal-ls %1").arg(dCacheCopy->errorString()));
		return;
	}

	//Wait for the end of the QProcess
	//!\ Might deadlock GUI
	dCacheCopy->waitForFinished();

	if(dCacheCopy->exitCode() != 0)
	{
		emit log("ERROR", dCacheCopy->errorString());
	}
}


//--------------------------------------------------------------------------------------------------------------

void dCacheTools::Check()
{
	/*
	 * Check Method
	 * Call GFAL-LS to check if the file exist on the dCache
	 */

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

		Checkfilename = filename;//store filename

		/**
		 * Call the QProcess method
		 */
		this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);
	}
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::Check(QString srm, QString base, QString output, QString file)
{
	/*
	 * Method to call the QProcess to check
	 * Avoid deadlock GUI by using QProcess signals
	 */

	dCacheCopy = new QProcess();
	dCacheCopy->setProcessChannelMode(QProcess::SeparateChannels);//No stdout on console

	/**
	 * GFAL-LS command
	 */
	std::string str = "/usr/bin/gfal-ls -l ";
	str += srm.toStdString();
	str += base.toStdString();
	str += "/";
	str += output.toStdString();
	str += file.toStdString();

	//Start of the QProcess
	dCacheCopy->start(QString::fromStdString(str));

	//Wait for the start of the QProcess
	if(!dCacheCopy->waitForStarted())
	{
		emit log("ERROR", QString("gfal-ls %1").arg(dCacheCopy->errorString()));
	}

	//Connect the signal finished of the QProcess to the slot to handle when the QProcess is finished
	connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedCheck(int, QProcess::ExitStatus)));
	indexfile++;//Increment the file index
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::finishedCheck(int exitCode, QProcess::ExitStatus exitStatus)
{
	/*
	 * Method called when QProcess is finished (Check)
	 * Handles when copy check and simple check
	 */

	if(m_check)//Case Check
	{
		if(exitCode != 0)
			emit log("WARNING", QString("File %1 is not on dCache").arg(Checkfilename));
		else
			emit log("INFO", QString("File %1 is on dCache").arg(Checkfilename));

		//Continue to check until no more files in the list
		if(indexfile < nfiles)
		{
			QFileInfo fileInfo = list.at(indexfile);
			QString filename = fileInfo.fileName();

			Checkfilename = filename;

			//Start QProcess again
			this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);
		}
	}

	if(m_copy)//Case Copy
	{
		/**
		 * Set isOndCache to true or false for the copy case
		 */

		if(exitCode != 0)
		{
			emit log("WARNING", QString("File %1 is not on dCache").arg(Checkfilename));
			isOndCache = false;
		}
		else
			isOndCache = true;
	}
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::Copy()
{
	/*
	 * Method called for the copy to the dCache
	 * Avoid deadlock GUI by using QProcess signals
	 */

	QLocale::setDefault(QLocale::English);//set the locale to english

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
		if(m_stop)//Stop case
			return;

		emit log("INFO", "Starting Copying..");//Logging

		idxProcess = 0;//Process index

		QFileInfo fileInfo = list.at(idxProcess);

		/*
		 * Get file informations : size, name and last modified time
		 */
		QString filename = fileInfo.fileName();
		QDateTime fileTime = fileInfo.lastModified();
		QDateTime current = QDateTime::currentDateTime();
		qint64 filesize = fileInfo.size();

		emit log("DEBUG", QString("File : %1 \n "
				"Last Modification : %2 \n "
				"Current time : %3").arg(filename, fileTime.toString(Qt::LocalDate), current.toString(Qt::LocalDate)));//Logging

		/*
		 * Check time of the file
		 * Needs 5 minutes difference with current time
		 */
		if(current.toTime_t() - fileTime.toTime_t() < 350)
		{
			emit log("DEBUG", "File too young. Will be copied later");
			idxProcess++;//Increment Process index
			this->goToNextFile();//Process next file
			return;
		}

		/*
		 * Check if file size is over 100kb
		 */
		if(filesize < 100000)
		{
			emit log("DEBUG", QString("File too small %1  kb. Will be copied later").arg(QString::number(filesize)));
			idxProcess++;//Increment Process index
			this->goToNextFile();//Process next file
			return;
		}

		/*
		 * GFAL-COPY command
		 */
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

		//Check if the file exist already on the dCache
		this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);

		//Get Run number
		lastrunNumber = StripRunNumber(filename, m_type);
		//Set filename
		Checkfilename = filename;

		//Delay the QThread by 1 sec
		this->delay(Threaddelay/Threaddelay);

		if(!isOndCache)//Case no file on dCache
		{
			dCacheCopy = new QProcess();
			dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);
			dCacheCopy->start(QString::fromStdString(str));

			if(!dCacheCopy->waitForStarted())
			{
				emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
				return;
			}

			/*
			 * Connect the signal finished of the QProcess to the slot to handle it when finished
			 */
			connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
			idxProcess++;//Increment Process index
		}
		else
		{
			emit log("DEBUG", QString("File %1 is already on dCache").arg(filename));
			idxProcess++;//Increment Process index
			this->goToNextFile();//Process next file
			return;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::finishedProcess(int exitCode, QProcess::ExitStatus exitStatus)
{
	/*
	 * Method to handle when QProcess finishes (Copy)
	 */

	emit log("INFO", QString("Completed with exit code %1").arg(QString::number(exitCode)));//Logging

	this->delay(Threaddelay);//Delay QThread by 5 secs

	//Continue if more files to upload
	if(idxProcess < nfiles)
	{
		//Stop case
		if(m_stop)
			return;

		QFileInfo fileInfo = list.at(idxProcess);

		/*
		 * Get file informations : size, name and last modified time
		 */
		QString filename = fileInfo.fileName();
		QDateTime fileTime = fileInfo.lastModified();
		QDateTime current = QDateTime::currentDateTime();
		qint64 filesize = fileInfo.size();

		emit log("DEBUG", QString("File : %1 \n "
				"Last Modification : %2 \n "
				"Current time : %3").arg(filename, fileTime.toString(Qt::LocalDate), current.toString(Qt::LocalDate)));

		/*
		 * Check time of the file
		 * Needs 5 minutes difference with current time
		 */
		if(current.toTime_t() - fileTime.toTime_t() < 350)
		{
			emit log("DEBUG", "File too young. Going to next file.");
			idxProcess++;//Increment Process index
			this->goToNextFile();//Go to next file
			return;
		}

		/*
		 * Check if file size is over 100kb
		 */
		if(filesize < 100000)
		{
			emit log("DEBUG", QString("File too small %1 kb. Will be copied later").arg(QString::number(filesize)));
			idxProcess++;//Increment Process index
			this->goToNextFile();//Go to next file
			return;
		}

		/*
		 * GFAL-COPY command
		 */
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

		//Check if the file exist already on the dCache
		this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);

		//Get Run number
		lastrunNumber = StripRunNumber(filename, m_type);
		//Set filename
		Checkfilename = filename;

		//Delay the QThread by 1 sec
		this->delay(Threaddelay/Threaddelay);

		//Case no file on dCache
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

			/*
			 * Connect the signal finished of the QProcess to the slot to handle it when finished
			 */
			connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
			idxProcess++;//Increment Process index
		}
		else
		{
			emit log("DEBUG", QString("File %1 is already on dCache").arg(filename));
			idxProcess++;//Increment Process index
			this->goToNextFile();//Process next file
			return;
		}
	}
	else
		this->start();//Start again to copy (looping)
}

//--------------------------------------------------------------------------------------------------------------

void dCacheTools::goToNextFile()
{
	/*
	 * Similar method as previous
	 * Handles case to go to next file
	 */

	this->delay(Threaddelay);//Delay QThread by 5 secs

	if(idxProcess < nfiles)
	{
		if(m_stop)
			return;

		QFileInfo fileInfo = list.at(idxProcess);

		/*
		 * Get file informations : size, name and last modified time
		 */
		QString filename = fileInfo.fileName();
		QDateTime fileTime = fileInfo.lastModified();
		QDateTime current = QDateTime::currentDateTime();
		qint64 filesize = fileInfo.size();

		emit log("DEBUG", QString("File : %1 \n "
				"Last Modification : %2 \n "
				"Current time : %3").arg(filename, fileTime.toString(Qt::LocalDate), current.toString(Qt::LocalDate)));

		/*
		 * Check time of the file
		 * Needs 5 minutes difference with current time
		 */
		if(current.toTime_t() - fileTime.toTime_t() < 350)
		{
			emit log("DEBUG", "File too young. Going to next file.");
			idxProcess++;//Increment Process index
			this->goToNextFile();//Process next file
			return;
		}

		/*
		 * Check if file size is over 100kb
		 */
		if(filesize < 100000)
		{
			emit log("DEBUG", QString("File too small %1 kb. Will be copied later").arg(QString::number(filesize)));
			idxProcess++;//Increment Process index
			this->goToNextFile();//Process next file
			return;
		}

		/*
		 * GFAL-COPY command
		 */
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

		//Check if the file exist already on the dCache
		this->Check("srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/", m_base, m_output, filename);

		//Get run number
		lastrunNumber = StripRunNumber(filename, m_type);
		//Set filename
		Checkfilename = filename;

		//Delay the QThread by 1 sec
		this->delay(Threaddelay/Threaddelay);

		//Case no file on dCache
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

			/*
			 * Connect the signal finished of the QProcess to the slot to handle it when finished
			 */
			connect(dCacheCopy, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
			idxProcess++;//Increment Process index
		}
		else
		{
			emit log("DEBUG", QString("File %1 is already on dCache").arg(filename));
			idxProcess++;//Increment Process index
			this->goToNextFile();//Process next file
			return;
		}
	}
	else
		this->start();//Start again to copy (looping)
}

//--------------------------------------------------------------------------------------------------------------


