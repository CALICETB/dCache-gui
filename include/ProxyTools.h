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
 * @brief The proxy tool class to interact handle the proxy operations
 */

#ifndef PROXYTOOLS_H
#define PROXYTOOLS_H

//-- Qt headers
#include <QString>
#include <QProcess>

/*
 * ProxyTools Class
 * Class handling the proxy operations
 * Creates, Check and Destroy a grid proxy using voms-proxy-init, voms-proxy-info
 */

class ProxyTools : public QObject
{
	Q_OBJECT
public:

	/**
	 * Constructor
	 */
	ProxyTools();

	/**
	 * Destructor
	 */
	~ProxyTools();

	/*
	 * Method to set the password
	 * @param pass Password from the QDialog
	 * @return void
	 */
	void setPassword(QString pass) {_password = pass;}

	/*
	 * Set the environment of the QProcess using the native env
	 * @return void
	 */
	void SetEnv();

	signals:

	/**
	 * Signal to be sent to the logger widget
	 * @param type Type of logging message (INFO, WARNING, ERROR, DEBUG).
	 * @param message Message to send to the logger
	 * @return void
	 */
	void log(QString type, QString message);

	/**
	 * Signal to send to the GUI to ask for the password
	 * @return void
	 */
	void PasswordRequired();

	/**
	 * Signal to send to the proxy status to the GUI
	 * @param status Timeleft of the proxy
	 * @return void
	 */
	void ProxyStatus(QString status);

	/**
	 * Signal to send when the proxy is destroyed
	 * @return void
	 */
	void ProxyDestroyed();

	public slots:

	/**
	 * Method to start the proxy
	 */
	void StartProxy();

	/**
	 * Method to check the proxy validity
	 */
	void CheckProxy();

	/**
	 * Method to destroy the proxy
	 */
	void DestroyProxy(int timeleft);

	private:

	QProcess *proxytool; /**< QProcess to do the operations */
	QProcessEnvironment env; /**< QProcess environment */
	QString _password; /**< private member to get the password and write it in the QProcess */

};

#endif // PROXYTOOLS_H
