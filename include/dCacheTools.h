#ifndef DCACHETOOLS_H
#define DCACHETOOLS_H

#include <QString>
#include <QThread>
#include <QProcess>

class dCacheTools : public QThread
{
    Q_OBJECT
public:
    dCacheTools();
    ~dCacheTools();

    void run();
    void setPassword(QString pass) {_password = pass;}

signals:
    void log(QString type, QString message);
    void PasswordRequired();
    void ProxyStatus(QString status);
    void readyRead(QProcess *proc, QString proc_name);

public slots:
    void StartProxy();
    void CheckProxy();
    void DoList(QString dir);
    void readStdOut(QProcess *proc, QString proc_name);

private:
    QProcess *startproxy, *checkproxy, *list;
    QString _password;
};

#endif // DCACHETOOLS_H
