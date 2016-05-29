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

public slots:
    void StartProxy();
    void CheckProxy();

private:
    QProcess *startproxy, *checkproxy;
    QString _password;
};

#endif // DCACHETOOLS_H
