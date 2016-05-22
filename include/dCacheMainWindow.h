#ifndef DCACHEMAINWINDOW_H
#define DCACHEMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class dCacheMainWindow;
}

class dCacheMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit dCacheMainWindow(QWidget *parent = 0);
    ~dCacheMainWindow();

private:
    Ui::dCacheMainWindow *ui;
};

#endif // DCACHEMAINWINDOW_H
