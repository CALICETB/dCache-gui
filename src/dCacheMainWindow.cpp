#include "dCacheMainWindow.h"
#include "ui_dcachemainwindow.h"

#include <QInputDialog>

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
    connect(m_tools, SIGNAL(PasswordRequired()), this, SLOT(showPassword()));

    emit log("MESSAGE", "dCache-GUI started");

    m_logger->show();
    m_tools->run();

    connect(ui->StartProxy, SIGNAL(clicked(bool)), m_tools, SLOT(StartProxy()));
    connect(ui->CheckProxy, SIGNAL(clicked(bool)), m_tools, SLOT(CheckProxy()));
    connect(ui->Configure, SIGNAL(clicked(bool)), this, SLOT(Configure()));
    connect(ui->Exit, SIGNAL(clicked(bool)), m_logger, SLOT(close()));
    connect(ui->Exit, SIGNAL(clicked(bool)), this, SLOT(close()));

    ui->StartCopy->setEnabled(false);
    ui->StopCopy->setEnabled(false);
    ui->ListFiles->setEnabled(false);
}

dCacheMainWindow::~dCacheMainWindow()
{
    emit log("MESSAGE", "dCache-GUI exiting");
    m_logger->deleteLater();
    m_tools->terminate();

    delete ui;
}

void dCacheMainWindow::Configure()
{
    InputDir = ui->InputDir->text();
    BaseDir = ui->BaseDir->text();
    OutputDir = ui->OutputDir->text();

    isLabview = ui->LabviewCheck->isChecked();
    isEUDAQ = ui->EUDAQCheck->isChecked();
    isLED = ui->LEDCheck->isChecked();

    if( (InputDir.isEmpty() || BaseDir.isEmpty() || OutputDir.isEmpty()) || (isLabview == false && isEUDAQ  == false && isLED == false) )
    {
        emit log("WARNING", "No settings specified");
    }
    else
    {
        emit log("DEBUG", "dCache settings :");
        emit log("DEBUG", QString("Input Dir %1, Base Dir %2, Output Dir %3").arg(InputDir, BaseDir, OutputDir));

        ui->StartCopy->setEnabled(true);
        ui->StopCopy->setEnabled(true);
        ui->ListFiles->setEnabled(true);
    }
}

void dCacheMainWindow::showPassword()
{
    bool ok;
    QInputDialog* inputDialog = new QInputDialog();
    inputDialog->setOptions(QInputDialog::NoButtons);
    QString password = inputDialog->getText(this, tr("Enter password"),
                                            tr("Password:"), QLineEdit::Password,
                                            tr(""), &ok);

    if(ok && !password.isEmpty())
    {
        m_tools->setPassword(password);
    }
}
