#include "Logger.h"

//Qt headers
#include <QHBoxLayout>
#include <QScrollBar>

//-----------------------------------------------------------------------------------------------

Logger::Logger()
{
	//Set size of the QWidget
    setMaximumWidth(1000);
    setMaximumHeight(250);

    this->resize(this->maximumWidth(), this->maximumHeight());
    this->setWindowTitle("dCache Logger");//Title of the widget

    //set layout
    QHBoxLayout *pLoggingLayout = new QHBoxLayout();
    setLayout(pLoggingLayout);

    //Read Only Log
    m_pLoggingWidget = new QTextEdit();
    m_pLoggingWidget->setReadOnly(true);
    pLoggingLayout->addWidget(m_pLoggingWidget);
}

//-----------------------------------------------------------------------------------------------

Logger::Logger(QWidget *parent)
{
	//Set size of the QWidget
    setMaximumWidth(1000);
    setMaximumHeight(250);

    this->resize(this->maximumWidth(), this->maximumHeight());
    this->setWindowTitle("dCache Logger");//Title of the widget

    //set layout
    QHBoxLayout *pLoggingLayout = new QHBoxLayout();
    setLayout(pLoggingLayout);

    //Read Only Log
    m_pLoggingWidget = new QTextEdit();
    m_pLoggingWidget->setReadOnly(true);
    pLoggingLayout->addWidget(m_pLoggingWidget);
}

//-----------------------------------------------------------------------------------------------

Logger::~Logger()
{
    m_pLoggingWidget->deleteLater();//Cleaning
}

//-----------------------------------------------------------------------------------------------

void Logger::Log(QString type, QString message)
{
	/**
	 * Does the printing of the message on the widget
	 */

    QColor color = getColor(type);

    // scroll to the end of text edit
    m_pLoggingWidget->horizontalScrollBar()->setValue(m_pLoggingWidget->horizontalScrollBar()->maximum());

    // set the cursor at the end
    QTextCursor cursor = m_pLoggingWidget->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    m_pLoggingWidget->setTextCursor(cursor);

    // put the message
    m_pLoggingWidget->setTextColor(QColor(color));
    m_pLoggingWidget->insertPlainText("[");
    m_pLoggingWidget->insertPlainText(type);
    m_pLoggingWidget->insertPlainText("]");
    m_pLoggingWidget->setTextColor(QColor(Qt::black));
    m_pLoggingWidget->insertPlainText(" ");
    m_pLoggingWidget->insertPlainText(message);
    m_pLoggingWidget->insertPlainText("\n");
}

//-----------------------------------------------------------------------------------------------

QColor Logger::getColor(QString type)
{
	/**
	 * Handles the different type of messages and assign them a color
	 */

    if(type == "ERROR")
        return QColor(Qt::red);
    else if(type == "MESSAGE")
        return QColor(Qt::blue);
    else if(type == "DEBUG")
        return QColor(Qt::blue);
    else if(type == "INFO")
        return QColor(Qt::green);
    else if(type == "WARNING")
        return QColor(255,165,0);
    else
        return QColor(Qt::black);
}
