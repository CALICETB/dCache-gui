#include "dCacheMainWindow.h"
#include "ui_dcachemainwindow.h"

dCacheMainWindow::dCacheMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dCacheMainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("dCache GUI");
    m_logger.show();

    connect(this, SIGNAL(log(QString,QString)), &m_logger, SLOT(Log(QString,QString)));
    connect(ui->Exit, SIGNAL(clicked(bool)), &m_logger, SLOT(close()));
    connect(ui->Exit, SIGNAL(clicked(bool)), this, SLOT(close()));

    emit log("MESSAGE", "dCache-GUI started");
}

dCacheMainWindow::~dCacheMainWindow()
{
    emit log("MESSAGE", "dCache-GUI exiting");
    delete ui;
}
