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
 * @brief The GUI class to interact with the user
 */

#ifndef DCACHEMAINWINDOW_H
#define DCACHEMAINWINDOW_H

//-- Qt headers
#include <QMainWindow>
#include <QTimer>

//-- Objects
#include "Logger.h"
#include "dCacheTools.h"
#include "ProxyTools.h"

namespace Ui {
class dCacheMainWindow;
}

class dCacheMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	/**
	 * Constructor for the GUI.
	 * @param parent Parent QWidget.
	 */
	explicit dCacheMainWindow(QWidget *parent = 0);

	/**
	 * Destructor for the GUI.
	 */
	~dCacheMainWindow();

	/**
	 * Initialization method
	 */
	void init();

	/**
	 * Signals
	 */
	signals :

	/**
	 * Signal to be sent to the logger widget
	 * @param type Type of logging message (INFO, WARNING, ERROR, DEBUG).
	 * @param message Message to send to the logger
	 * @return void
	 */
	void log(QString type, QString message);

	/**
	 * Signal to be sent to the Thread for configuration
	 * @param InputDir Input directory or file to be copied/check on the dCache
	 * @param BaseDir Base directory on the dCache i.e. tb-desy/native/desyAhcal2016
	 * @param OutputDir Output directory on the dCache where to put the files to upload or check
	 * @param type Type of files to upload (txt, slcio, raw or other type)
	 * @param isSingleFile Boolean to notify the thread that it is a single file to upload
	 * @return void
	 */
	void Configure_dCacheTool(QString InputDir, QString BaseDir, QString OutputDir, int type, bool isSingleFile);

	/**
	 * Private slots
	 */
	private slots:

	/**
	 * Method when the look for directory button is clicked
	 * @return void
	 */
	void on_toolButton_clicked();

	/**
	 * Method when the Configure button is clicked
	 * @return void
	 */
	void Configure();

	/**
	 * Method when the password for the proxy is asked
	 * @return void
	 */
	void showPassword();

	/**
	 * Method to get the timeleft of the proxy
	 * @param status Timeleft of the proxy
	 * @return void
	 */
	void updateProxy(QString status);

	/**
	 * Method to update the GUI
	 * @return void
	 */
	void updateMainWindow();

	/**
	 * Method to set a parameter when the Thread is running
	 * @return void
	 */
	void ThreadRunning();

	/**
	 * Method to set a parameter when the Thread is stopped
	 * @return void
	 */
	void ThreadStopped();

	/**
	 * Method when the Start Copy button is clicked
	 * @return void
	 */
	void StartCopy();

	/**
	 * Method when the Stop Copy button is clicked
	 * @return void
	 */
	void StopCopy();

	/**
	 * Method when the List Files button is clicked
	 * @return void
	 */
	void ListFiles();

	/**
	 * Method when the Check Copy button is clicked
	 * @return void
	 */
	void CheckCopy();

	/**
	 * Method Method when the Exit button is clicked
	 * @return void
	 */
	void Close();

	/**
	 * Private variables
	 */
	private:

	Ui::dCacheMainWindow *ui; /**< UI pointer */

	Logger *m_logger; /**< Logger widget object */
	dCacheTools *m_tools; /**< dCacheTools object */
	ProxyTools *m_proxy; /**< Proxy object */

	QTimer *timer; /**< Timer object */

	QString InputDir; /**< Variable to store the input directory/file */
	QString BaseDir;  /**< Variable to store the base directory */
	QString OutputDir; /**< Variable to store the output directory */

	bool isLabview; /**< Boolean is set to true if file is text file */
	bool isEUDAQ; /**< Boolean is set to true if file is slcio file */
	bool isLED; /**< Boolean is set to true if file is text file */
	bool isRaw; /**< Boolean is set to true if file is raw binary file */
	bool isOther; /**< Boolean is set to true if file is another type of file */
	bool isSingleFile; /**< Boolean is set to true if upload a single file */
	bool m_running; /**< Boolean set to true if dCacheTools Thread is running */

	int timeleft; /**< Variable to store the timeleft of the proxy */
	int timertime; /**< Variable to set the timer to update the GUI */
	int type; /**< Variable to store the type of the file (1 text files, 2 slcio files, 3 raw binary and 4 for other types) */

};

#endif // DCACHEMAINWINDOW_H
