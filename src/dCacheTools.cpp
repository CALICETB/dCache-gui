#include "dCacheTools.h"

#include <QLocale>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

#include <boost/filesystem.hpp>

dCacheTools::dCacheTools()
{
    _password = "";
    dCachetool = new QProcess();
    dCachetool->setProcessChannelMode(QProcess::SeparateChannels);
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
        dCachetool->waitForFinished(-1);

        if(dCachetool->exitCode() == 0)
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

void dCacheTools::CheckProxy()
{
    dCachetool->setProcessChannelMode(QProcess::SeparateChannels);

    dCachetool->start("/usr/bin/voms-proxy-info --timeleft");

    if(!dCachetool->waitForStarted())
    {
        emit log("ERROR", QString("voms-proxy-info %1").arg(dCachetool->errorString()));
        return;
    }
    dCachetool->waitForFinished(-1);

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

    dCachetool->waitForFinished(-1);

    if(dCachetool->exitCode() != 0)
    {
        emit log("ERROR", dCachetool->errorString());
    }
}

void dCacheTools::SetEnv()
{
    env = QProcessEnvironment::systemEnvironment();
    dCachetool->setProcessEnvironment(env);
}

void dCacheTools::Copy(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile)
{
    emit log("MESSAGE", "gfal-cp");

    QLocale::setDefault(QLocale::English);
    dCachetool->setProcessChannelMode(QProcess::ForwardedChannels);

    std::string str;

    if(isSingleFile)
    {
        boost::filesystem::path p(Input.toStdString());
        std::string filename = p.filename().string();

        str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
        str += "file:/";
        str += Input.toStdString();
        str += " srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
        str += BaseDir.toStdString();
        str += "/";
        str += OutputDir.toStdString();
        str += filename;
        /*
        str += " lfn:/grid/calice/";
        str += BaseDir.toStdString();
        str += "/";
        str += OutputDir.toStdString();
        str += filename;
        */

        emit log("DEBUG", QString::fromStdString(str));

        dCachetool->startDetached(QString::fromStdString(str));

        if(!dCachetool->waitForStarted())
        {
            emit log("ERROR", QString("gfal-copy %1").arg(dCachetool->errorString()));
            return;
        }

        dCachetool->waitForFinished(-1);

        if(dCachetool->exitCode() != 0)
        {
            emit log("ERROR", dCachetool->errorString());
        }
    }
    else
    {
        QString filetype;
        if(type == 1)
            filetype = "txt";
        if(type == 2)
            filetype = "slcio";
        if(type == 3)
            filetype = "raw";

        QDir dir(Input);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        dir.setSorting(QDir::Time | QDir::Reversed);

        QFileInfoList list = dir.entryInfoList();

        for (int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list.at(i);
            emit log("DEBUG", QString("file : %1 \t Time : %2").arg(fileInfo.fileName(), (fileInfo.lastModified()).toString(Qt::ISODate)));

            std::string filename = (fileInfo.fileName()).toStdString();
            if(fileInfo.completeSuffix() != filetype && type != 4) continue;

            str = "/usr/bin/gfal-copy --dry-run -n 5 -t 6000 ";
            str += "file:/";
            str += Input.toStdString();
            str += "/";
            str += filename;
            str += " srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
            str += BaseDir.toStdString();
            str += "/";
            str += OutputDir.toStdString();
            str += filename;
            /*
            str += " lfn:/grid/calice/";
            str += BaseDir.toStdString();
            str += "/";
            str += OutputDir.toStdString();
            str += filename;
            */

            emit log("DEBUG", QString::fromStdString(str));

            dCachetool->startDetached(QString::fromStdString(str));

            if(!dCachetool->waitForStarted())
            {
                emit log("ERROR", QString("gfal-copy %1").arg(dCachetool->errorString()));
                //return;
            }

            dCachetool->waitForFinished(-1);

            if(dCachetool->exitCode() != 0)
            {
                emit log("ERROR", dCachetool->errorString());
            }
        }
    }
}

void dCacheTools::StopCopy()
{
    emit log("INFO", "Waiting for last Copy");
    dCachetool->waitForFinished(-1);
}

void dCacheTools::DestroyProxy(int timeleft)
{
    dCachetool->waitForFinished(-1);

    if(timeleft > 0)
    {
        emit log("INFO", "Destroying Proxy");

        QString proxy = "/bin/rm ";
        proxy.append(env.value("X509_USER_PROXY"));

        dCachetool->start(proxy);

        if(!dCachetool->waitForStarted())
        {
            emit log("ERROR", QString("Destroy Proxy %1").arg(dCachetool->errorString()));
            return;
        }

        dCachetool->waitForFinished(-1);

        if(dCachetool->exitCode() != 0)
        {
            emit log("ERROR", dCachetool->errorString());
        }
    }

    emit ProxyDestroyed();
}
