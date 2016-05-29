#ifndef DCACHEMAINWINDOW_H
#define DCACHEMAINWINDOW_H

#include <QMainWindow>

#include "Logger.h"
#include "dCacheTools.h"

namespace Ui {
class dCacheMainWindow;
}

class dCacheMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit dCacheMainWindow(QWidget *parent = 0);
    ~dCacheMainWindow();

signals :
    //Signal to Logegr
    void log(QString type, QString message);

public slots:
    void Configure();
    void showPassword();
    void updateProxy();

private:
    Ui::dCacheMainWindow *ui;
    Logger *m_logger;
    dCacheTools *m_tools;

    QString InputDir, BaseDir, OutputDir;
    bool isLabview, isEUDAQ, isLED;

};

#endif // DCACHEMAINWINDOW_H
