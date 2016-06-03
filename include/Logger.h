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
 * @brief The logger class
 */

#ifndef LOGGER_H
#define LOGGER_H

//-- Qt headers
#include <QObject>
#include <QGroupBox>
#include <QColor>
#include <QTextEdit>

/*
 * Logger Class
 * Class handling the logging of the software
 * Capture every message emitted by all Modules, display DEBUG, VERBOSE and NORMAL messages
 */

class Logger : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor of the QWidget
	 */
	Logger();

	/**
	 *  Destructor of the QWidget
	 */
	~Logger();

	/**
	 * Change color depending on message type
	 * @param type Type of message
	 * @return QColor
	 */
	QColor getColor(QString type);

	signals:

	public slots:

	/**
	 * Slot to receive messages to display
	 * @param type Type of message
	 * @param message Message to log
	 * @return void
	 */
	void Log(QString type, QString message);

	private:

	QTextEdit *m_pLoggingWidget;	/**< Object to display the log messages */

};

#endif // LOGGER_H
