#include "dCacheMainWindow.h"
#include "ui_dCacheMainWindow.h"

#include <QInputDialog>
#include <QFileDialog>

dCacheMainWindow::dCacheMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dCacheMainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("dCache GUI");

    m_logger = new Logger();
    m_tools = new dCacheTools();

    connect(this, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));
    connect(this, SIGNAL(DoList(QString)), m_tools, SLOT(DoList(QString)));

    connect(m_tools, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));
    connect(m_tools, SIGNAL(PasswordRequired()), this, SLOT(showPassword()));
    connect(m_tools, SIGNAL(ProxyStatus(QString)), this, SLOT(updateProxy(QString)));

    emit log("MESSAGE", "dCache-GUI started");

    m_logger->show();
    m_tools->run();

    connect(ui->StartProxy, SIGNAL(clicked(bool)), m_tools, SLOT(StartProxy()));
    connect(ui->CheckProxy, SIGNAL(clicked(bool)), m_tools, SLOT(CheckProxy()));

    connect(ui->Configure, SIGNAL(clicked(bool)), this, SLOT(Configure()));
    connect(ui->StartCopy, SIGNAL(clicked(bool)), this, SLOT(StartCopy()));
    connect(ui->StopCopy, SIGNAL(clicked(bool)), this, SLOT(StopCopy()));
    connect(ui->ListFiles, SIGNAL(clicked(bool)), this, SLOT(ListFiles()));
    connect(ui->Exit, SIGNAL(clicked(bool)), this, SLOT(Close()));

    ui->StartCopy->setEnabled(false);
    ui->StopCopy->setEnabled(false);
    ui->ListFiles->setEnabled(false);

    ui->ProxyValid_label->setText("<font color='Red'>Check Proxy!</font>");
    ui->BaseDir->setText("tb-desy/native/desyAhcal2016/AHCAL_Testbeam_Raw_May_2016");
}

dCacheMainWindow::~dCacheMainWindow()
{
    delete ui;
}

void dCacheMainWindow::Configure()
{
    InputDir = ui->InputDir->text();
    BaseDir = ui->BaseDir->text();
    OutputDir = ui->OutputDir->text();

    isSingleFile = ui->SingleCheck->isChecked();
    isLabview = ui->LabviewCheck->isChecked();
    isEUDAQ = ui->EUDAQCheck->isChecked();
    isLED = ui->LEDCheck->isChecked();
    isRaw = ui->RawCheck->isChecked();

    if(isSingleFile == true)
    {
        if((!InputDir.isEmpty() && !BaseDir.isEmpty()))
        {
            emit log("MESSAGE", "dCache Copy Settings :");
            if(!OutputDir.isEmpty())
                emit log("MESSAGE", QString("Input Dir %1\n\tBase Dir %2\n\tOutput Dir %3").arg(InputDir, BaseDir, OutputDir));
            else
                emit log("MESSAGE", QString("Input Dir %1\n\tBase Dir %2\n\tOutput Dir None").arg(InputDir, BaseDir));

            ui->StartCopy->setEnabled(true);
            ui->StopCopy->setEnabled(false);
            ui->ListFiles->setEnabled(true);
            ui->Configure->setEnabled(false);
        }
        else
            emit log("WARNING", "Settings are missing");
    }
    else
    {
        if( ((InputDir.isEmpty() || BaseDir.isEmpty()) || (isLabview == false && isEUDAQ  == false && isLED == false && isRaw == false)) )
        {
            emit log("WARNING", "No settings specified. Is it a single file upload ??");
            return;
        }
        else
        {
            emit log("MESSAGE", "dCache Copy Settings :");
            if(!OutputDir.isEmpty())
                emit log("MESSAGE", QString("Input Dir %1\n\tBase Dir %2\n\tOutput Dir %3").arg(InputDir, BaseDir, OutputDir));
            else
                emit log("MESSAGE", QString("Input Dir %1\n\tBase Dir %2\n\tOutput Dir None").arg(InputDir, BaseDir));

            ui->StartCopy->setEnabled(true);
            ui->StopCopy->setEnabled(false);
            ui->ListFiles->setEnabled(true);
            ui->Configure->setEnabled(false);
        }
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

void dCacheMainWindow::updateProxy(QString status)
{
    ui->ProxyValid_label->setText(status);
}

void dCacheMainWindow::on_toolButton_clicked()
{
    if(ui->SingleCheck->isChecked())
    {
        QString file = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                                    "/home/calice",
                                                    tr("files : *.*"));

        ui->InputDir->setText(file);
    }
    else
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
                                                        "/home/calice",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

        ui->InputDir->setText(dir);
    }
}

void dCacheMainWindow::StartCopy()
{
    ui->StartCopy->setEnabled(false);
    ui->StopCopy->setEnabled(true);
    ui->Configure->setEnabled(false);

    emit log("MESSAGE", "Start Copy");
}

void dCacheMainWindow::ListFiles()
{
    emit log("MESSAGE", "Listing files");
    emit DoList(ui->BaseDir->text());
}

void dCacheMainWindow::StopCopy()
{
    ui->StartCopy->setEnabled(true);
    ui->StopCopy->setEnabled(false);
    ui->Configure->setEnabled(true);

    emit log("MESSAGE", "Stop Copy");
}

void dCacheMainWindow::Close()
{
    emit log("MESSAGE", "dCache-GUI exiting");
    m_logger->close();
    m_tools->terminate();

    delete m_tools;
    delete m_logger;

    this->close();
}
