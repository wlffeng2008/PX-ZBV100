#include "LogToFile.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDebug>

namespace file_log
{
    QFile m_logFile;
    const qint64 m_MaxFileSize = 5 * 1024 * 1024; // 5MB
    bool  bWriteToFile = true;
    void setWriteToFile(bool bSet){ bWriteToFile = bSet ; }

    bool createNewLogFile()
    {
        static QString m_logDirPath = QCoreApplication::applicationDirPath() + "/log";
        QDir logDir( m_logDirPath );
        if( !logDir.exists() )
            logDir.mkpath( m_logDirPath );

        QString strTime = QDateTime::currentDateTime().toString( "yyyyMMdd_hhmmss" );
        QString strFile = QString( "%1/log_%2.txt" ).arg(m_logDirPath, strTime);

        if ( m_logFile.isOpen() )
            m_logFile.close();

        m_logFile.setFileName( strFile );
        if( !m_logFile.open( QIODevice::Append | QIODevice::Text ) )
            return false;

        return true;
    }

    bool checkLogFileSize()
    {
        if ( m_logFile.size() > m_MaxFileSize )
        {
            if( createNewLogFile() )
                return true;
        }
        return false;
    }

    QtMessageHandler pOldHandler = nullptr ;

    static void LogMessageHandler( QtMsgType type, const QMessageLogContext &context, const QString &msg )
    {
        if(bWriteToFile)
        {
            QString strTime = QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss.zzz: " );
            QString strType ;
            switch ( type )
            {
            case QtDebugMsg:    strType = "DEBUG: ";    break;
            case QtWarningMsg:  strType = "WARNING: ";  break;
            case QtCriticalMsg: strType = "CRITICAL: "; break;
            case QtFatalMsg:    strType = "FATAL: ";    break;
            case QtInfoMsg:     strType = "INFO: ";     break;
            }

            checkLogFileSize();
            QTextStream out( &m_logFile );
            out << strTime << strType << "\n" ;
        }

        if(pOldHandler) pOldHandler(type, context, msg) ;
    }

    bool LogInit()
    {
        createNewLogFile();

        pOldHandler = qInstallMessageHandler( LogMessageHandler );
        return true;
    }

}
