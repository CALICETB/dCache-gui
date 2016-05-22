#include "dCacheMainWindow.h"
#include "ui_dcachemainwindow.h"

dCacheMainWindow::dCacheMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dCacheMainWindow)
{
    ui->setupUi(this);
}

dCacheMainWindow::~dCacheMainWindow()
{
    delete ui;
}
