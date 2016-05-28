#include "Logger.h"

#include <QHBoxLayout>
#include <QScrollBar>

Logger::Logger()
{
    setMaximumWidth(1000);
    setMaximumHeight(250);

    this->resize(this->maximumWidth(), this->maximumHeight());
    this->setWindowTitle("dCache Logger");

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
    m_pLoggingWidget->deleteLater();
}

//-----------------------------------------------------------------------------------------------

void Logger::Log(QString type, QString message)
{
    Qt::GlobalColor color = getColor(type);

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

Qt::GlobalColor Logger::getColor(QString type)
{
    if(type == "ERROR")
        return Qt::red;
    else if(type == "MESSAGE")
        return Qt::magenta;
    else if(type == "DEBUG")
        return Qt::blue;
    else if(type == "VERBOSE")
        return Qt::green;
    else
        return Qt::black;
}
