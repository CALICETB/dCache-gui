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

    timertime = 1000;
    timeleft = 0;
    type = -1;

    connect(this, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));
    connect(this, SIGNAL(DoListing(QString)), m_tools, SLOT(DoList(QString)));
    connect(this, SIGNAL(DoCopy(QString, QString, QString, int, bool)), m_tools, SLOT(Copy(QString, QString, QString, int, bool)));
    connect(this, SIGNAL(DoStopCopy()), m_tools, SLOT(StopCopy()));

    connect(m_tools, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));
    connect(m_tools, SIGNAL(PasswordRequired()), this, SLOT(showPassword()));
    connect(m_tools, SIGNAL(ProxyStatus(QString)), this, SLOT(updateProxy(QString)));
    connect(m_tools, SIGNAL(ProxyDestroyed()), m_logger, SLOT(close()));
    connect(m_tools, SIGNAL(ProxyDestroyed()), m_tools, SLOT(quit()));

    emit log("MESSAGE", "dCache-GUI started");

    m_logger->show();
    m_tools->start();

    connect(ui->StartProxy, SIGNAL(clicked(bool)), m_tools, SLOT(StartProxy()));
    connect(ui->CheckProxy, SIGNAL(clicked(bool)), m_tools, SLOT(CheckProxy()));

    connect(ui->Configure, SIGNAL(clicked(bool)), this, SLOT(Configure()));
    connect(ui->StartCopy, SIGNAL(clicked(bool)), this, SLOT(StartCopy()));
    connect(ui->StopCopy, SIGNAL(clicked(bool)), this, SLOT(StopCopy()));
    connect(ui->CheckCopy, SIGNAL(clicked(bool)), this, SLOT(CheckCopy()));
    connect(ui->ListFiles, SIGNAL(clicked(bool)), this, SLOT(ListFiles()));
    connect(ui->Exit, SIGNAL(clicked(bool)), this, SLOT(Close()));

    ui->CheckProxy->setEnabled(false);
    ui->StartCopy->setEnabled(false);
    ui->StopCopy->setEnabled(false);
    ui->ListFiles->setEnabled(false);
    ui->toolButton->setEnabled(false);
    ui->InputDir->setReadOnly(true);
    ui->BaseDir->setReadOnly(true);
    ui->OutputDir->setReadOnly(true);
    ui->Configure->setEnabled(false);

    ui->ProxyValid_label->setText("<font color='Red'>Check Proxy!</font>");
    ui->BaseDir->setText("tb-desy/native/desyAhcal2016/AHCAL_Testbeam_Raw_May_2016");

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMainWindow()));
    timer->start(timertime);
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
        if( ((InputDir.isEmpty() || BaseDir.isEmpty()) || type == -1) )
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
    //emit log("INFO", status);
    timeleft = status.toInt();

    this->updateMainWindow();
}

void dCacheMainWindow::on_toolButton_clicked()
{
    QString file;

    if(isSingleFile)
    {
        if(type == 1)
            file = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                                "/home/calice",
                                                tr("Labview file : *.txt"));

        else if(type == 2)
            file = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                                "/home/calice",
                                                tr("EUDAQ file : *.slcio"));
        else if(type == 3)
            file = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                                "/home/calice",
                                                tr("Raw file : *.raw"));
        else
            file = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                                "/home/calice",
                                                tr("Any type : *.*"));
    }
    else
    {
        file = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
                                                 "/home/calice",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    }

    ui->InputDir->setText(file);
}

void dCacheMainWindow::StartCopy()
{
    ui->StartCopy->setEnabled(false);
    ui->StopCopy->setEnabled(true);
    ui->Configure->setEnabled(false);

    ui->InputDir->setReadOnly(true);
    ui->BaseDir->setReadOnly(true);
    ui->OutputDir->setReadOnly(true);

    emit DoCopy(ui->InputDir->text(), ui->BaseDir->text(), ui->OutputDir->text(), type, isSingleFile);
}

void dCacheMainWindow::ListFiles()
{
    emit DoListing(ui->BaseDir->text());
}

void dCacheMainWindow::StopCopy()
{
    ui->StartCopy->setEnabled(true);
    ui->StopCopy->setEnabled(false);
    ui->Configure->setEnabled(true);

    ui->InputDir->setReadOnly(false);
    ui->BaseDir->setReadOnly(false);
    ui->OutputDir->setReadOnly(false);

    emit log("MESSAGE", "Stop Copy");

    emit DoStopCopy();
}

void dCacheMainWindow::CheckCopy()
{


}

void dCacheMainWindow::Close()
{
    emit log("MESSAGE", "dCache-GUI exiting");

    m_tools->StopCopy();
    m_tools->DestroyProxy(timeleft);

    delete m_tools;
    delete m_logger;

    this->close();
}

void dCacheMainWindow::updateMainWindow()
{
    isSingleFile = ui->SingleCheck->isChecked();
    isLabview = ui->LabviewCheck->isChecked();
    isEUDAQ = ui->EUDAQCheck->isChecked();
    isLED = ui->LEDCheck->isChecked();
    isRaw = ui->RawCheck->isChecked();
    isOther = ui->OtherCheck->isChecked();

    if(isLabview || isLED)
        type = 1;
    if(isEUDAQ)
        type = 2;
    if(isRaw)
        type = 3;
    if(isOther)
        type = 4;

    if(type != -1)
    {
        ui->toolButton->setEnabled(true);
        ui->InputDir->setReadOnly(false);
        ui->BaseDir->setReadOnly(false);
        ui->OutputDir->setReadOnly(false);

        ui->ListFiles->setEnabled(true);
        ui->Configure->setEnabled(true);
    }

    if(timeleft == 0)
    {
        ui->StartCopy->setEnabled(false);
        ui->StopCopy->setEnabled(false);
        ui->CheckCopy->setEnabled(false);
        ui->ListFiles->setEnabled(false);
        ui->Configure->setEnabled(false);
        ui->InputDir->setReadOnly(true);
        ui->BaseDir->setReadOnly(true);
        ui->OutputDir->setReadOnly(true);

        ui->ProxyValid_label->setText("<font color='Red'>Check Proxy!</font>");
    }
    else
    {
        ui->CheckProxy->setEnabled(true);
        ui->Configure->setEnabled(true);
        ui->ListFiles->setEnabled(true);

        timeleft = timeleft - timertime/1000;
        int hours = timeleft/3600;
        int minutes = timeleft%3600/60;

        if(timeleft%2 == 0)
            ui->ProxyValid_label->setText(QString("%1 : %2 time left").arg(QString::number(hours), QString::number(minutes)));
        else
            ui->ProxyValid_label->setText(QString("%1   %2 time left").arg(QString::number(hours), QString::number(minutes)));
    }
}
