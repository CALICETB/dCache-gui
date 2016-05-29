#include "dCacheTools.h"

dCacheTools::dCacheTools()
{
    _password = "";
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
    args << "-verify -voms calice:/calice/Role=production";

    startproxy->start("/usr/bin/voms-proxy-init", args);
    if(!startproxy->waitForStarted())
    {
        emit log("ERROR", QString("voms-proxy-init %1").arg(startproxy->errorString()));
        return;
    }

    emit PasswordRequired();

    if(!_password.isEmpty())
    {
        emit log("VERBOSE", "Password received");
        startproxy->write(_password.toStdString().data());
        startproxy->closeWriteChannel();
    }
}

void dCacheTools::CheckProxy()
{
    emit log("MESSAGE", "dCache-GUI : Check Proxy Validity");

}
