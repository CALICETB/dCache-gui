#ifndef DCACHETOOLS_H
#define DCACHETOOLS_H

#include "QString.h"
#include "QThread.h"

class dCacheTools : public QThread
{
    Q_OBJECT
public:
    dCacheTools();
    ~dCacheTools();

    void run();

signals:
     void log(QString type, QString message);

public slots:
     void StartProxy();
     void CheckProxy();
};

#endif // DCACHETOOLS_H
