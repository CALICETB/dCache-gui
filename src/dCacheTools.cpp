#include "dCacheTools.h"

#include <QDebug>

dCacheTools::dCacheTools()
{
    _password = "";

    connect(this, SIGNAL(readyRead(QProcess*, QString)), this, SLOT(readStdOut(QProcess*, QString)));
}

dCacheTools::~dCacheTools()
{


}

void dCacheTools::run()
{

}

void dCacheTools::StartProxy()
{
    emit log("MESSAGE", "dCache-GUI : Start Proxy");
    startproxy = new QProcess();
    startproxy->setProcessChannelMode(QProcess::ForwardedOutputChannel);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("GRID_SECURITY_DIR", "/etc/grid-security");
    env.insert("X509_CERT_DIR", env.value("GRID_SECURITY_DIR") + "/certificates");
    env.insert("X509_VOMS_DIR", env.value("GRID_SECURITY_DIR") + "/vomsdir");
    env.insert("VOMS_USERCONF", "/etc/vomses/vomses");
    env.insert("X509_USER_PROXY", env.value("HOME") + "/k5-ca-proxy.pem");
    env.insert("GLOBUS_LOCATION", "/usr");
    env.insert("SRM_PATH", "/usr/share/srm");
    env.insert("PERL5LIB", "/usr/share/perl5");
    env.insert("PYTHONPATH", env.value("PYTHONPATH") + ":/usr/lib64/python2.6/site-packages:/usr/lib/python2.6/site-packages");
    env.insert("PATH", "/usr/sbin:" + env.value("PATH"));
    env.insert("LD_PRELOAD", env.value("LD_PRELOAD") + ":/usr/lib64/libpdcap.so");
    env.insert("DCACHE_IO_TUNNEL", "/usr/lib64/dcap/libgsiTunnel.so");
    env.insert("DCACHE_CLIENT_ACTIVE", "1");
    env.insert("LCG_CATALOG_TYPE", "lfc");
    env.insert("LFC_HOST", "grid-lfc.desy.de");

    startproxy->setProcessEnvironment(env);

    QStringList args;
    args << " -verify -voms calice:/calice/Role=production";

    startproxy->start("/usr/bin/voms-proxy-init", args);
    if(!startproxy->waitForStarted())
    {
        emit log("ERROR", QString("voms-proxy-init %1").arg(startproxy->errorString()));
        return;
    }

    emit PasswordRequired();

    if(!_password.isEmpty())
    {
        emit log("INFO", "Password received");
        startproxy->write(_password.toStdString().data());
        startproxy->closeWriteChannel();
        startproxy->waitForFinished();

        if(startproxy->exitCode() == 0)
        {
            emit ProxyStatus("<font color=Green> OK </font>");
            emit readyRead(startproxy, "StartProxy");
            startproxy->deleteLater();
        }
        else
        {
            emit ProxyStatus(startproxy->errorString());
            startproxy->deleteLater();
        }
    }
}

void dCacheTools::CheckProxy()
{
    emit log("MESSAGE", "dCache-GUI : Check Proxy Validity");
    checkproxy = new QProcess();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("GRID_SECURITY_DIR", "/etc/grid-security");
    env.insert("X509_CERT_DIR", env.value("GRID_SECURITY_DIR") + "/certificates");
    env.insert("X509_VOMS_DIR", env.value("GRID_SECURITY_DIR") + "/vomsdir");
    env.insert("VOMS_USERCONF", "/etc/vomses/vomses");
    env.insert("X509_USER_PROXY", env.value("HOME") + "/k5-ca-proxy.pem");
    env.insert("GLOBUS_LOCATION", "/usr");
    env.insert("SRM_PATH", "/usr/share/srm");
    env.insert("PERL5LIB", "/usr/share/perl5");
    env.insert("PYTHONPATH", env.value("PYTHONPATH") + ":/usr/lib64/python2.6/site-packages:/usr/lib/python2.6/site-packages");
    env.insert("PATH", "/usr/sbin:" + env.value("PATH"));
    env.insert("LD_PRELOAD", env.value("LD_PRELOAD") + ":/usr/lib64/libpdcap.so");
    env.insert("DCACHE_IO_TUNNEL", "/usr/lib64/dcap/libgsiTunnel.so");
    env.insert("DCACHE_CLIENT_ACTIVE", "1");
    env.insert("LCG_CATALOG_TYPE", "lfc");
    env.insert("LFC_HOST", "grid-lfc.desy.de");

    checkproxy->setProcessEnvironment(env);

    QStringList arguments;
    arguments << " --timeleft";

    checkproxy->start("/usr/bin/voms-proxy-info", arguments);

    if(!checkproxy->waitForStarted())
    {
        emit log("ERROR", QString("voms-proxy-info %1").arg(checkproxy->errorString()));
        return;
    }
    checkproxy->waitForFinished();
    emit readyRead(checkproxy, "CheckProxy");
    checkproxy->deleteLater();
}

void dCacheTools::DoList(QString dir)
{
    emit log("INFO", "Listing called");

    list = new QProcess();
    list->setProcessChannelMode(QProcess::ForwardedChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("GRID_SECURITY_DIR", "/etc/grid-security");
    env.insert("X509_CERT_DIR", env.value("GRID_SECURITY_DIR") + "/certificates");
    env.insert("X509_VOMS_DIR", env.value("GRID_SECURITY_DIR") + "/vomsdir");
    env.insert("VOMS_USERCONF", "/etc/vomses/vomses");
    env.insert("X509_USER_PROXY", env.value("HOME") + "/k5-ca-proxy.pem");
    env.insert("GLOBUS_LOCATION", "/usr");
    env.insert("SRM_PATH", "/usr/share/srm");
    env.insert("PERL5LIB", "/usr/share/perl5");
    env.insert("PYTHONPATH", env.value("PYTHONPATH") + ":/usr/lib64/python2.6/site-packages:/usr/lib/python2.6/site-packages");
    env.insert("PATH", "/usr/sbin:" + env.value("PATH"));
    env.insert("LD_PRELOAD", env.value("LD_PRELOAD") + ":/usr/lib64/libpdcap.so");
    env.insert("DCACHE_IO_TUNNEL", "/usr/lib64/dcap/libgsiTunnel.so");
    env.insert("DCACHE_CLIENT_ACTIVE", "1");
    env.insert("LCG_CATALOG_TYPE", "lfc");
    env.insert("LFC_HOST", "grid-lfc.desy.de");

    list->setProcessEnvironment(env);

    std::string str = " -l ";
    str += "srm://dcache-se-desy.desy.de/pnfs/desy.de/calice/";
    str += dir.toStdString();

    QStringList arguments;
    arguments << QString::fromStdString(str);

    emit log("DEBUG", arguments.join(""));

    list->start("/usr/bin/gfal-ls", arguments);

    if(!list->waitForStarted())
    {
        emit log("ERROR", QString("gfal-ls %1").arg(list->errorString()));
        return;
    }
    list->waitForFinished();

    if(list->exitCode() == 0)
    {
        emit readyRead(list, "ListDir");
        list->deleteLater();
    }
    else
    {
        emit log("ERROR", list->errorString());
        list->deleteLater();
    }



}

void dCacheTools::readStdOut(QProcess *proc, QString proc_name)
{
    if(proc_name == "CheckProxy")
    {
        proc->setReadChannel(QProcess::StandardOutput);
        QTextStream stream(proc);

        while (!stream.atEnd())
        {
            QString line = stream.readLine();

            int found = line.indexOf("time", 0, Qt::CaseInsensitive);
            if(found == 0)
                emit log("INFO", line);
        }
    }
    else
    {
        proc->setReadChannel(QProcess::StandardOutput);
        QTextStream stream(proc);

        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            emit log("INFO", line);
        }
    }
}
