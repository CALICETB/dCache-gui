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
 * @brief dCacheTools class
 */

#ifndef DCACHETOOLS_H
#define DCACHETOOLS_H

//-- Qt headers
#include <QString>
#include <QThread>
#include <QProcess>
#include <QFileInfoList>

/*
 * dCacheTools Class
 * Class handling the dCache operations in a different thread
 * Copy, Checks and List operation for dCache. Uses gfal commands.
 */

class dCacheTools : public QThread
{
	Q_OBJECT
public:

	/*
	 * Constructor
	 */
	dCacheTools();

	/**
	 * Destructor
	 */
	~dCacheTools();

	/**
	 * Virtual Overloaded run method of QThread
	 * @return void
	 */
	virtual void run();

	/**
	 * Method to Copy a file on the dCache
	 * @return void
	 */
	void Copy();

	/**
	 * Method to Check a file on the dCache
	 * @return void
	 */
	void Check();

	/**
	 * Method to list files on the dCache
	 * @return void
	 */
	void List();

	/**
	 * Set the flags for the QThread
	 * @param copy Boolean set to true if Copy
	 * @param check Boolean set to true if checking
	 * @param Boolean set to true if listing
	 * @return void
	 */
	void setFlags(bool copy, bool check, bool list)
	{
		m_copy = copy;
		m_check = check;
		m_list = list;
	}

	/**
	 * Set the stop flag for the QThread
	 * @param stop Boolean set to true if the user click the stop button
	 * @return void
	 */
	void setStopFlag(bool stop) {m_stop = stop;}

	/**
	 * Get the run Number of the file
	 * @return int
	 */
	int getRunNumber() {return lastrunNumber;}

	signals:

	/**
	 * Signal to be sent to the logger widget
	 * @param type Type of logging message (INFO, WARNING, ERROR, DEBUG).
	 * @param message Message to send to the logger
	 * @return void
	 */
	void log(QString type, QString message);

	/**
	 * Signal sent by the QThread when started
	 */
	void started();

	/**
	 * Signal sent by the QThread when finished
	 * (Going out of the start method)
	 */
	void finished();

	public slots:

	/**
	 * Configure the different parameters needed for the upload/check/listing
	 * @param Input Input file of directory
	 * @param BaseDir Base directory on the dCache
	 * @param OutputDir Output directory on the dCache
	 * @param type Type of the file
	 * @param isSingleFile Boolean set to true if single file upload
	 * @return void
	 */
	void Configure(QString Input, QString BaseDir, QString OutputDir, int type, bool isSingleFile);

	/**
	 * Overloaded slot for QThread start()
	 * Start the QThread
	 */
	void start();

	/**
	 * Slot when a QProcess (Copy) is finished to continue the next copy
	 * avoiding deadlocking of the GUI
	 */
	void finishedProcess (int exitCode, QProcess::ExitStatus exitStatus);

	/**
	 * Slot when a QProcess (Check) is finished to continue the next file to check
	 * avoiding deadlocking of the GUI
	 */
	void finishedCheck(int exitCode, QProcess::ExitStatus exitStatus);

	protected:

	/**
	 * Method to delay the QThread
	 * @param secs Number of seconds for delay
	 * @return void
	 */
	void delay(int secs);

	/**
	 * Method to get the run number of a file
	 * @param filename Name of the file to be uploaded
	 * @param type Type of the file
	 * @return int
	 */
	int StripRunNumber(QString filename, int type);

	/**
	 * Check Method
	 * @param srm QString for the SRM i.e. srm://dcache-se-desy.desy.de/ for DESY
	 * @param base Base directory on the dCache
	 * @param output Output directory on the dCache
	 * @param file File to be checked
	 * @return void
	 */
	void Check(QString srm, QString base, QString output, QString file);

	/**
	 * Method to go to the next file to upload
	 * if the file is either too young or the size if too small (100kb)
	 * @return void
	 */
	void goToNextFile();

	private:

	QProcess *dCacheCopy; /**< QProcess for the dCache operations */

	bool m_copy; /**< Boolean set to true if copy */
	bool m_check; /**< Boolean set to true if checking */
	bool m_list; /**< Boolean set to true if listing */
	bool m_isSingleFile; /**< Boolean set to true if single file upload */
	bool m_stop; /**< Boolean set to true if the user stops the QThread */
	bool isOndCache; /**< Boolean set to true if the file is already on dCache */
	bool dry_run; /**< Boolean set to true if testing */

	QString m_dir; /**< Variable containing the input file or directory */
	QString m_base; /**< Variable containing the base directory on the dCache */
	QString m_output; /**< Variable containing the output directory on the dCache */
	QString Checkfilename; /**< Variable containing the filename when Checking */

	int idxProcess; /**< Index of the QProcess */
	int nfiles; /**< Number of files to upload */
	int Threaddelay; /**< Delay time for the QThread */
	int lastrunNumber; /**< Variable containing the run number of the file being uploaded */
	int m_type; /**< Type of the file */
	int indexfile; /**< Index of the file when checking */

	QFileInfoList list; /**< List containing informations on the files to upload */
};

#endif // DCACHETOOLS_H
