#include "ProxyTools.h"

ProxyTools::ProxyTools()
{

_password = "";
b_start = false;
b_check = false;
b_destroy = false;
proxytool = new QProcess();

}

ProxyTools::~ProxyTools()
{


}

void ProxyTools::StartProxy()
{
	this->SetEnv();
	emit log("MESSAGE", "dCache-GUI : Start Proxy");

	proxytool->setProcessChannelMode(QProcess::SeparateChannels);

	proxytool->start("/usr/bin/voms-proxy-init -verify -voms calice:/calice/Role=production");
	if(!proxytool->waitForStarted())
	{
		emit log("ERROR", QString("voms-proxy-init %1").arg(proxytool->errorString()));
		return;
	}

	emit PasswordRequired();

	if(!_password.isEmpty())
	{
		emit log("INFO", "Password received");
		proxytool->write(_password.toStdString().data());
		proxytool->closeWriteChannel();
		proxytool->waitForFinished();

		if(proxytool->exitCode() == 0)
		{
			int timeleft = 60*60*12;
			emit ProxyStatus(QString::number(timeleft));
		}
		else
		{
			int timeleft = 0;
			emit ProxyStatus(QString::number(timeleft));
		}
	}
}

void ProxyTools::CheckProxy()
{
	proxytool->setProcessChannelMode(QProcess::SeparateChannels);

	proxytool->start("/usr/bin/voms-proxy-info --timeleft");

	if(!proxytool->waitForStarted())
	{
		emit log("ERROR", QString("voms-proxy-info %1").arg(proxytool->errorString()));
		return;
	}
	proxytool->waitForFinished();

	if(proxytool->exitCode() == 0)
	{
		int timeleft = atoi(proxytool->readAllStandardOutput().data());
		emit ProxyStatus(QString::number(timeleft));
	}
	else
	{
		emit log("ERROR", proxytool->errorString());
	}
}

void ProxyTools::DestroyProxy(int timeleft)
{
	proxytool->waitForFinished();

	if(timeleft > 0)
	{
		emit log("INFO", "Destroying Proxy");

		QString proxy = "/bin/rm ";
		proxy.append(env.value("X509_USER_PROXY"));

		proxytool->start(proxy);

		if(!proxytool->waitForStarted())
		{
			emit log("ERROR", QString("Destroy Proxy %1").arg(proxytool->errorString()));
			return;
		}

		proxytool->waitForFinished();

		if(proxytool->exitCode() != 0)
		{
			emit log("ERROR", proxytool->errorString());
		}
	}

	emit ProxyDestroyed();
}

void ProxyTools::SetEnv()
{
	env = QProcessEnvironment::systemEnvironment();
    proxytool->setProcessEnvironment(env);
}
