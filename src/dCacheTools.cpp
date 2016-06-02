#include "dCacheTools.h"

#include <QLocale>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

#include <boost/filesystem.hpp>

dCacheTools::dCacheTools()
{
    dCacheCopy = new QProcess();
    dCacheCopy->setProcessChannelMode(QProcess::SeparateChannels);
}

dCacheTools::~dCacheTools()
{


}

void dCacheTools::run()
{

}


void dCacheTools::DoList(QString dir)
{
    emit log("INFO", "Listing called");

    dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);

    std::string str = "/usr/bin/gfal-ls -l ";
    str += "srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
    str += dir.toStdString();

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

void dCacheTools::Copy(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile)
{
    emit log("MESSAGE", "gfal-cp");

    QLocale::setDefault(QLocale::English);
    dCacheCopy->setProcessChannelMode(QProcess::ForwardedChannels);

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

        dCacheCopy->start(QString::fromStdString(str));

        if(!dCacheCopy->waitForStarted())
        {
            emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
            return;
        }

        dCacheCopy->waitForFinished();

        if(dCacheCopy->exitCode() != 0)
        {
            emit log("ERROR", dCacheCopy->errorString());
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

            dCacheCopy->start(QString::fromStdString(str));

            if(!dCacheCopy->waitForStarted())
            {
                emit log("ERROR", QString("gfal-copy %1").arg(dCacheCopy->errorString()));
                //return;
            }

            dCacheCopy->waitForFinished();

            if(dCacheCopy->exitCode() != 0)
            {
                emit log("ERROR", dCacheCopy->errorString());
            }
        }
    }
}

void dCacheTools::StopCopy()
{
    emit log("INFO", "Waiting for last Copy");
    dCacheCopy->waitForFinished();
}
