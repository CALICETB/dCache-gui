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

#include "ProxyTools.h"

//--------------------------------------------------------------------------------------------------------------

ProxyTools::ProxyTools()
{
	/**
	 * Constructor
	 */
	_password = "";//empty password
	proxytool = new QProcess();//Initialization of QProcess

}

//--------------------------------------------------------------------------------------------------------------

ProxyTools::~ProxyTools()
{
	//Cleaning
	proxytool->deleteLater();

}

//--------------------------------------------------------------------------------------------------------------

void ProxyTools::StartProxy()
{
	/**
	 * Start the proxy
	 */

	this->SetEnv();

	emit log("MESSAGE", "dCache-GUI : Start Proxy");//Logging

	/**
	 * Start the QProcess
	 */
	proxytool->setProcessChannelMode(QProcess::SeparateChannels);
	proxytool->start("/usr/bin/voms-proxy-init -verify -voms calice:/calice/Role=production");//Special calice production role

	//Wait for the process to be started
	if(!proxytool->waitForStarted())
	{
		emit log("ERROR", QString("voms-proxy-init %1").arg(proxytool->errorString()));
		return;
	}

	//Ask for the password
	emit PasswordRequired();

	if(!_password.isEmpty())
	{
		//Write the password to the QProcess and close the channel
		proxytool->write(_password.toStdString().data());
		proxytool->closeWriteChannel();

		//Wait for the end of the QProcess
		proxytool->waitForFinished();

		if(proxytool->exitCode() == 0)
		{
			int timeleft = 60*60*12;//default time of 12h
			emit log("INFO", "Grid Proxy created for 12h");//Logging
			emit ProxyStatus(QString::number(timeleft));
		}
		else
		{
			int timeleft = 0;
			emit log("ERROR", QString("Exit Code %1 - Error in Proxy creation").arg(QString::number(proxytool->exitCode())));//Logging
			emit ProxyStatus(QString::number(timeleft));
		}
	}
	else
	{
		emit log("ERROR", "No password entered!");//Logging
		return;
	}
}

//--------------------------------------------------------------------------------------------------------------

void ProxyTools::CheckProxy()
{
	/**
	 * Check proxy validity
	 */

	proxytool->setProcessChannelMode(QProcess::SeparateChannels);
	proxytool->start("/usr/bin/voms-proxy-info --timeleft");

	//Wait for start of the QProcess
	if(!proxytool->waitForStarted())
	{
		emit log("ERROR", QString("voms-proxy-info %1").arg(proxytool->errorString()));
		return;
	}

	//Wait for the end of the QProcess
	proxytool->waitForFinished();

	if(proxytool->exitCode() == 0)
	{
		int timeleft = atoi(proxytool->readAllStandardOutput().data());//read the timeleft from stdout
		emit ProxyStatus(QString::number(timeleft));
	}
	else
	{
		emit log("ERROR", proxytool->errorString());
		return;
	}
}

//--------------------------------------------------------------------------------------------------------------

void ProxyTools::DestroyProxy(int timeleft)
{
	/**
	 * Destroy the proxy
	 */

	if(timeleft < 2*3600)//Destroy if less than 2 hours left
	{
		emit log("INFO", "Destroying Proxy");

		QString proxy = "/bin/rm ";
		proxy.append(env.value("X509_USER_PROXY"));

		//Start the QProcess
		proxytool->start(proxy);

		//Wait for the start of the QProcess
		if(!proxytool->waitForStarted())
		{
			emit log("ERROR", QString("Destroy Proxy %1").arg(proxytool->errorString()));
			return;
		}

		//Wait for the end of the QProcess
		proxytool->waitForFinished();

		if(proxytool->exitCode() != 0)
		{
			emit log("ERROR", proxytool->errorString());
		}
	}

	emit ProxyDestroyed();//emit signal proxy destroyed
}

//--------------------------------------------------------------------------------------------------------------

void ProxyTools::SetEnv()
{
	/*
	 * Set the QProcess Environment
	 */

	env = QProcessEnvironment::systemEnvironment();
	proxytool->setProcessEnvironment(env);
}

//--------------------------------------------------------------------------------------------------------------


