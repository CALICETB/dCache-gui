/*
 * Author Eldwan Brianne
 * July 2015
 * PhD DESY
*/

#ifndef LOGGER_H
#define LOGGER_H

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
    //Constructor
    Logger();
    //Destructor
    ~Logger();

    //Change color depending on message type
    QColor getColor(QString type);

signals:

public slots:
    //Slot to receive messages to display
    void Log(QString type, QString message);

private:
    //Display for messages
    QTextEdit *m_pLoggingWidget;
};

#endif // LOGGER_H
