#ifndef PROXYTOOLS_H
#define PROXYTOOLS_H

#include <QString>
#include <QThread>
#include <QProcess>

class ProxyTools : public QThread
{
	Q_OBJECT
public:
	ProxyTools();
	~ProxyTools();

	void run();
	void setPassword(QString pass) {_password = pass;}
	void SetEnv();

	signals:
	void log(QString type, QString message);
	void PasswordRequired();
	void ProxyStatus(QString status);
	void ProxyDestroyed();

	public slots:
	void StartProxy();
	void CheckProxy();
	void DestroyProxy(int timeleft);

	private:
	QProcess *proxytool;
	QProcessEnvironment env;
	QString _password;
	bool b_start, b_check, b_destroy;
};

#endif // PROXYTOOLS_H
