/**
 * @file
 * @author  Eldwan Brianne <eldwan.brianne@desy.de>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 *
 */

#include "dCacheMainWindow.h"
#include "ui_dCacheMainWindow.h"

//-- Qt headers
#include <QInputDialog>
#include <QFileDialog>
#include <QEventLoop>

//--------------------------------------------------------------------------------------------------------------

dCacheMainWindow::dCacheMainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::dCacheMainWindow)
{
	ui->setupUi(this);
	this->setWindowTitle("dCache GUI"); //set title of the GUI

	m_logger = new Logger(); //init Logger Widget
	m_proxy = new ProxyTools(); //init Proxy handler object

	timertime = 500; //timer to update the GUI in ms

	/**
	 * Initialization of the member variables
	 */
	timeleft = 0;
	type = -1;
	InputDir = "";
	BaseDir = "";
	OutputDir = "";
	isLabview = false;
	isEUDAQ = false;
	isLED = false;
	isRaw = false;
	isOther = false;
	isSingleFile = false;
	type = -1;
	m_running = false;

	/**
	 * Connection of the signals and slots
	 */

	//Connect the GUI to forward message to the logger
	connect(this, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));

	//Connect the proxy object to forward message to the logger
	connect(m_proxy, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));
	//Connect the proxy to the GUI to prompt window when password is asked
	connect(m_proxy, SIGNAL(PasswordRequired()), this, SLOT(showPassword()));
	//Connect the proxy to the GUI to update the time of the proxy left
	connect(m_proxy, SIGNAL(ProxyStatus(QString)), this, SLOT(updateProxy(QString)));

	//Logging starting
	emit log("MESSAGE", "dCache-GUI started");

	m_logger->show();//Show the logger widget

	/**
	 * Connection of the different UI objects to slots
	 */
	connect(ui->StartProxy, SIGNAL(clicked(bool)), m_proxy, SLOT(StartProxy()));
	connect(ui->CheckProxy, SIGNAL(clicked(bool)), m_proxy, SLOT(CheckProxy()));

	connect(ui->Configure, SIGNAL(clicked(bool)), this, SLOT(Configure()));
	connect(ui->StartCopy, SIGNAL(clicked(bool)), this, SLOT(StartCopy()));
	connect(ui->StopCopy, SIGNAL(clicked(bool)), this, SLOT(StopCopy()));
	connect(ui->CheckCopy, SIGNAL(clicked(bool)), this, SLOT(CheckCopy()));
	connect(ui->ListFiles, SIGNAL(clicked(bool)), this, SLOT(ListFiles()));
	connect(ui->Exit, SIGNAL(clicked(bool)), this, SLOT(Close()));

	/**
	 * Initialization of the state of the buttons and input text
	 */
	ui->StartCopy->setEnabled(false);
	ui->StopCopy->setEnabled(false);
	ui->ListFiles->setEnabled(false);
	ui->toolButton->setEnabled(false);
	ui->InputDir->setReadOnly(true);
	ui->BaseDir->setReadOnly(true);
	ui->OutputDir->setReadOnly(true);
	ui->Configure->setEnabled(false);

	/**
	 * Initialization of some labels on the GUI
	 */
	ui->ProxyValid_label->setText("<font color='Red'>Check Proxy!</font>");
	//Native base directory for calice
	ui->BaseDir->setText("tb-desy/native/");
	ui->LastRun->setText(".....");

	/**
	 * Setup the timer and connect the timeout signal to the slot that updates the GUI
	 */
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(updateMainWindow()));
	timer->start(timertime);//Updates every 500 ms
}

//--------------------------------------------------------------------------------------------------------------

dCacheMainWindow::~dCacheMainWindow()
{
	delete ui;
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::init()
{
	/**
	 * Initialization of the QThread that handles dCache operations (Check, Listing and Copy)
	 * Connection of the different signals of the Thread to GUI slots
	 * emit the configuration of the directories to the QThread
	 */
	m_tools = new dCacheTools();
	connect(this, SIGNAL(Configure_dCacheTool(QString, QString, QString, int, bool)), m_tools, SLOT(Configure(QString, QString, QString, int, bool)));
	connect(m_tools, SIGNAL(log(QString,QString)), m_logger, SLOT(Log(QString,QString)));
	//Direct Connection to receive the signal before the end of the QThread
	connect(m_tools, SIGNAL(finished()), this, SLOT(ThreadStopped()), Qt::DirectConnection);
	connect(m_tools, SIGNAL(started()), this, SLOT(ThreadRunning()), Qt::DirectConnection);

	emit Configure_dCacheTool(InputDir, BaseDir, OutputDir, type, isSingleFile);
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::Configure()
{
	/**
	 * Configure button
	 * Get the different parameters and set them to their assigned members
	 */

	InputDir = ui->InputDir->text();//Input file or directory
	BaseDir = ui->BaseDir->text();//Base directory
	OutputDir = ui->OutputDir->text();//Output directory

	if(isSingleFile == true)//Case Single file
	{
		if((!InputDir.isEmpty() && !BaseDir.isEmpty()))
		{
			emit log("MESSAGE", "dCache Copy Settings :");
			if(!OutputDir.isEmpty())
				emit log("MESSAGE", QString("Input Directory : %1\n\t"
						"Base Directory : %2\n\t"
						"Output Directory : %3").arg(InputDir, BaseDir, OutputDir));
			else
				emit log("MESSAGE", QString("Input Directory : %1\n\t"
						"Base Directory : %2\n\t"
						"No Output directory specified").arg(InputDir, BaseDir));

			ui->StartCopy->setEnabled(true);
			ui->StopCopy->setEnabled(false);
			ui->CheckCopy->setEnabled(true);
			ui->ListFiles->setEnabled(true);
			ui->Configure->setEnabled(false);
		}
		else
			emit log("WARNING", "Settings are missing");//Input and Base directory missing
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
			/**
			 * Case when the configuration is fine
			 */

			emit log("MESSAGE", "dCache Copy Settings :");
			if(!OutputDir.isEmpty())
				emit log("MESSAGE", QString("Input Directory : %1\n\t"
						"Base Directory : %2\n\t"
						"Output Directory : %3").arg(InputDir, BaseDir, OutputDir));
			else
				emit log("MESSAGE", QString("Input Directory : %1\n\t"
						"Base Directory : %2\n\t"
						"No Output directory specified").arg(InputDir, BaseDir));

			ui->StartCopy->setEnabled(true);
			ui->StopCopy->setEnabled(false);
			ui->CheckCopy->setEnabled(true);
			ui->ListFiles->setEnabled(true);
			ui->Configure->setEnabled(false);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::showPassword()
{
	/**
	 * Show an input Dialog box to enter the proxy password
	 */

	bool ok;
	QInputDialog* inputDialog = new QInputDialog();
	inputDialog->setOptions(QInputDialog::NoButtons);
	QString password = inputDialog->getText(this, tr("Enter password"),
			tr("Password:"), QLineEdit::Password,
			tr(""), &ok);

	if(ok && !password.isEmpty())
	{
		m_proxy->setPassword(password);//send the password to the process that creates the grid proxy
	}
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::updateProxy(QString status)
{
	/**
	 * Get the timeleft of the proxy
	 */

	timeleft = status.toInt();

	this->updateMainWindow();//update the GUI
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::on_toolButton_clicked()
{
	/**
	 * Method to prompt a QFileDialog when searching for a directory or file to upload on the dCache
	 */

	QString file;

	//Case single file upload
	if(isSingleFile)
	{
		//LED or Labview files
		if(type == 1)
			file = QFileDialog::getOpenFileName(this, tr("Choose File"),
					"/home/calice",
					tr("Labview file : *.txt"));
		//slcio files
		else if(type == 2)
			file = QFileDialog::getOpenFileName(this, tr("Choose File"),
					"/home/calice",
					tr("EUDAQ file : *.slcio"));
		//raw binary files
		else if(type == 3)
			file = QFileDialog::getOpenFileName(this, tr("Choose File"),
					"/home/calice",
					tr("Raw file : *.raw"));
		//other files
		else
			file = QFileDialog::getOpenFileName(this, tr("Choose File"),
					"/home/calice",
					tr("Any type : *.*"));
	}
	//Case for directories
	else
	{
		file = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
				"/home/calice",
				QFileDialog::ShowDirsOnly
				| QFileDialog::DontResolveSymlinks);
	}

	ui->InputDir->setText(file);//Set the path into the GUI
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::StartCopy()
{
	/**
	 * Start Copy button clicked
	 * Setup the different buttons and start the QThread
	 */

	ui->StartCopy->setEnabled(false);
	ui->StopCopy->setEnabled(true);
	ui->Configure->setEnabled(false);
	ui->ListFiles->setEnabled(false);
	ui->CheckCopy->setEnabled(false);

	ui->InputDir->setReadOnly(true);
	ui->BaseDir->setReadOnly(true);
	ui->OutputDir->setReadOnly(true);

	this->init();//Initialization of the QThread
	m_tools->setFlags(true, false, false);//Set flags (Copy, Check, List)
	m_tools->start();//Start the QThread
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::ListFiles()
{
	/**
	 * List button clicked
	 * Start the QThread
	 */

	this->init();//Initialization of the QThread
	m_tools->setFlags(false, false, true);//Set flags (Copy, Check, List)
	m_tools->start();//Start the QThread
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::StopCopy()
{
	/**
	 * Stop Copy button clicked
	 * Setup the GUI buttons and send stop signal to the QThread
	 */

	ui->StartCopy->setEnabled(true);
	ui->StopCopy->setEnabled(false);
	ui->Configure->setEnabled(true);
	ui->ListFiles->setEnabled(true);
	ui->CheckCopy->setEnabled(true);

	ui->InputDir->setReadOnly(false);
	ui->BaseDir->setReadOnly(false);
	ui->OutputDir->setReadOnly(false);

	emit log("MESSAGE", "Stop Copy");//Logging
	m_tools->setStopFlag(true);//send stop signal to the QThread
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::CheckCopy()
{
	/**
	 * Check button clicked
	 * Set the flags and start the QThread
	 */

	this->init();//Initialization of the QThread
	m_tools->setFlags(false, true, false);//Set flags (Copy, Check, List)
	m_tools->start();//Start the QThread
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::Close()
{
	/**
	 * Exit button clicked
	 * Clean and exit the GUI
	 */

	emit log("MESSAGE", "dCache-GUI exiting");//Logging

	if(timeleft != 0)
		m_proxy->DestroyProxy(timeleft);//Destroy the proxy

	if(m_running)
		m_tools->quit();//Stop the QThread if running

	//Wait 2 secs
	QEventLoop loop;
	QTimer::singleShot(2000, &loop, SLOT(quit()));
	loop.exec();

	//Close the logger widget and GUI
	m_logger->close();
	this->close();

	//Cleaning
	delete m_tools;
	delete m_proxy;
	delete m_logger;
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::updateMainWindow()
{
	/**
	 * Updates the GUI
	 * Set the different buttons for different cases
	 */

	/**
	 * Check the type of files and if single file upload
	 */
	isSingleFile = ui->SingleCheck->isChecked();
	isLabview = ui->LabviewCheck->isChecked();
	isEUDAQ = ui->EUDAQCheck->isChecked();
	isLED = ui->LEDCheck->isChecked();
	isRaw = ui->RawCheck->isChecked();
	isOther = ui->OtherCheck->isChecked();

	//Define the type
	if(isLabview || isLED)
		type = 1;
	if(isEUDAQ)
		type = 2;
	if(isRaw)
		type = 3;
	if(isOther)
		type = 4;

	//Case no timeleft in the grid proxy
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
		//If type defined user can enter directory parameters
		if(type != -1)
		{
			ui->toolButton->setEnabled(true);
			ui->InputDir->setReadOnly(false);
			ui->BaseDir->setReadOnly(false);
			ui->OutputDir->setReadOnly(false);
		}

		//Case QThread is running avoiding interactions with the user
		if(!m_running)
		{
			ui->Configure->setEnabled(true);
			ui->ListFiles->setEnabled(true);
		}
		else
		{
			ui->Configure->setEnabled(false);
			ui->ListFiles->setEnabled(false);

			ui->LastRun->setText(QString::number(m_tools->getRunNumber()));
		}

		ui->CheckProxy->setEnabled(true);

		/**
		 * Updates the timeleft on the GUI
		 */
		timeleft = timeleft - timertime*2/1000;
		int hours = timeleft/3600;
		int minutes = timeleft%3600/60;

		if(timeleft%2 == 0)
			ui->ProxyValid_label->setText(QString("%1 : %2 time left").arg(QString::number(hours), QString::number(minutes)));
		else
			ui->ProxyValid_label->setText(QString("%1   %2 time left").arg(QString::number(hours), QString::number(minutes)));
	}
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::ThreadRunning()
{
	/**
	 * Case QThread is running
	 */
	m_running = true;
}

//--------------------------------------------------------------------------------------------------------------

void dCacheMainWindow::ThreadStopped()
{
	/**
	 * Case QThread is stopped
	 */
	m_running = false;
}
