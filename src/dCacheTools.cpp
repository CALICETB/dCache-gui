#include "dCacheTools.h"

#include <QDebug>

dCacheTools::dCacheTools()
{
    _password = "";
    dCachetool = new QProcess();
    dCachetool->setProcessChannelMode(QProcess::SeparateChannels);

    connect(this, SIGNAL(readyRead(QProcess*)), this, SLOT(readStdOut(QProcess*)));
}

dCacheTools::~dCacheTools()
{


}

void dCacheTools::run()
{

}

void dCacheTools::StartProxy()
{
    this->SetEnv();
    emit log("MESSAGE", "dCache-GUI : Start Proxy");

    dCachetool->setProcessChannelMode(QProcess::SeparateChannels);

    dCachetool->start("/usr/bin/voms-proxy-init -verify -voms calice:/calice/Role=production");
    if(!dCachetool->waitForStarted())
    {
        emit log("ERROR", QString("voms-proxy-init %1").arg(dCachetool->errorString()));
        return;
    }

    emit PasswordRequired();

    if(!_password.isEmpty())
    {
        emit log("INFO", "Password received");
        dCachetool->write(_password.toStdString().data());
        dCachetool->closeWriteChannel();
        dCachetool->waitForFinished();

        if(dCachetool->exitCode() == 0)
        {
            int timeleft = 60*60*12;
            emit ProxyStatus(QString::number(timeleft));
            emit readyRead(dCachetool);
        }
        else
        {
            emit ProxyStatus(dCachetool->errorString());
        }
    }
}

void dCacheTools::CheckProxy()
{
    dCachetool->setProcessChannelMode(QProcess::SeparateChannels);

    dCachetool->start("/usr/bin/voms-proxy-info --timeleft");

    if(!dCachetool->waitForStarted())
    {
        emit log("ERROR", QString("voms-proxy-info %1").arg(dCachetool->errorString()));
        return;
    }
    dCachetool->waitForFinished();

    if(dCachetool->exitCode() == 0)
    {
        int timeleft = atoi(dCachetool->readAllStandardOutput().data());
        emit ProxyStatus(QString::number(timeleft));
    }
    else
    {
        emit log("ERROR", dCachetool->errorString());
    }
}

void dCacheTools::DoList(QString dir)
{
    emit log("INFO", "Listing called");

    dCachetool->setProcessChannelMode(QProcess::ForwardedChannels);

    std::string str = "/usr/bin/gfal-ls -l ";
    str += "srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
    str += dir.toStdString();

    dCachetool->start(QString::fromStdString(str));

    if(!dCachetool->waitForStarted())
    {
        emit log("ERROR", QString("gfal-ls %1").arg(dCachetool->errorString()));
        return;
    }

    dCachetool->waitForFinished();

    if(dCachetool->exitCode() == 0)
    {
        emit readyRead(dCachetool);
    }
    else
    {
        emit log("ERROR", dCachetool->errorString());
    }
}

void dCacheTools::readStdOut(QProcess *proc)
{
    proc->setReadChannel(QProcess::StandardOutput);
    QTextStream stream(proc);

    while (!stream.atEnd())
    {
        QString line = stream.readLine();
    }
}

void dCacheTools::SetEnv()
{
    env = QProcessEnvironment::systemEnvironment();
    dCachetool->setProcessEnvironment(env);
}

void dCacheTools::Copy(QString Input, QString BaseDir, QString OutputDir, int type)
{
    emit log("MESSAGE", "gfal-cp");

}
