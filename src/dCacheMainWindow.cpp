#include "dCacheMainWindow.h"
#include "ui_dcachemainwindow.h"

dCacheMainWindow::dCacheMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dCacheMainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("dCache GUI");

    m_logger = new Logger();
    m_tools = new dCacheTools();

    connect(this, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));
    connect(m_tools, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));

    connect(ui->Exit, SIGNAL(clicked(bool)), m_logger, SLOT(close()));
    connect(ui->Exit, SIGNAL(clicked(bool)), this, SLOT(close()));

    emit log("MESSAGE", "dCache-GUI started");

    m_logger->show();
    m_tools->run();

    connect(ui->StartProxy, SIGNAL(clicked(bool)), m_tools, SLOT(StartProxy()));
    connect(ui->CheckProxy, SIGNAL(clicked(bool)), m_tools, SLOT(CheckProxy()));

}

dCacheMainWindow::~dCacheMainWindow()
{
    emit log("MESSAGE", "dCache-GUI exiting");
    m_logger->deleteLater();
    m_tools->terminate();

    delete ui;
}
