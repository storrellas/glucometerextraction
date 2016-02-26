#include "mainwindow.h"
#include <QApplication>

#include "commonparameters.h"

// Qt Includes
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QTime>
#include <QPlainTextEdit>

// Variable to ouput the log
QFile debuggingFile;

MainWindow* pMainWindow = NULL;

#define ENABLE_MESSAGE_HANDLER

/**
 * @brief nicMessageHandler Handles all qDebug
 * @param type Type of message sent
 * @param msg  Message sent
 */

void nicMessageHandlerVisual(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    QString line = msg;
//    if (!debuggingFile.open(QIODevice::WriteOnly | QIODevice::Append))
//        qDebug()<<"Error opening debuggingFile"<<debuggingFile.fileName()<<line;

    //in this function, you can write the message to any stream!
    switch (type) {
        case QtDebugMsg:
            line.insert(0,QTime::currentTime().toString("HH:mm:ss:zzz | "));
            line.append("\n");
            debuggingFile.write(line.toLatin1());
        break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s\n", msg.constData());
        break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s\n", msg.constData());
        break;
        case QtFatalMsg:
            line.insert(0,QTime::currentTime().toString("HH:mm:ss:zzz | FATAL "));
            line.append("\n");
            debuggingFile.write(line.toLatin1());
            //abort();
    }

    debuggingFile.flush();

    // Output to GUI
    QString lineGUI = QTime::currentTime().toString("HH:mm:ss:zzz | ") + msg;
    if(pMainWindow != NULL){
        pMainWindow->consoleWrite( lineGUI );
    }
/**/
    // Print also to console
    line = QTime::currentTime().toString("HH:mm:ss:zzz | ") + msg;
    fprintf(stderr, "%s\n", line.toLocal8Bit().data());
    fflush(stderr);

}

void nicMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    QString line = msg;
//    if (!debuggingFile.open(QIODevice::WriteOnly | QIODevice::Append))
//        qDebug()<<"Error opening debuggingFile"<<debuggingFile.fileName()<<line;

    //in this function, you can write the message to any stream!
    switch (type) {
        case QtDebugMsg:
            line.insert(0,QTime::currentTime().toString("HH:mm:ss:zzz | "));
            line.append("\n");
            debuggingFile.write(line.toLatin1());
        break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s\n", msg.constData());
        break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s\n", msg.constData());
        break;
        case QtFatalMsg:
            line.insert(0,QTime::currentTime().toString("HH:mm:ss:zzz | FATAL "));
            line.append("\n");
            debuggingFile.write(line.toLatin1());
            //abort();
    }

    debuggingFile.flush();

    // Print also to console
    line = QTime::currentTime().toString("HH:mm:ss:zzz | ") + msg;
    fprintf(stderr, "%s\n", line.toLocal8Bit().data());
    fflush(stderr);

}



/**
 * Configures the logging functionality
 */
void configureLoggging(){

    // Default log directory
    QString fileDir = "./";

    // Create directory
    QDir dir(fileDir);
    dir.setPath(fileDir);
    if (!dir.exists())
    {
        dir.mkpath(".");
        qDebug()<<"Does not exist"<<fileDir;
    }

    // Create file
    QString fileName = "qDebugLog_NICBenchamark2.txt";
    qDebug()<<"We output debug console to file:"<<fileName;
    debuggingFile.setFileName( fileDir + fileName);
    if (!debuggingFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        qDebug()<<"Error opening debuggingFile"<<debuggingFile.fileName();

    // Install the message handler for qDebug() calls
#ifdef ENABLE_MESSAGE_HANDLER
    qInstallMessageHandler(nicMessageHandlerVisual);
#endif
}


int main(int argc, char *argv[])
{
    qDebug() << "-- Simple Manager Start --";



    // Run application
    QApplication a(argc, argv);


    MainWindow mainWindow;
    mainWindow.nicMessageHandlerVisual = nicMessageHandlerVisual;
    mainWindow.nicMessageHandler = nicMessageHandler;
    mainWindow.show();
    pMainWindow = &mainWindow;




    // Configure logging
    configureLoggging();


    loggerMacroDebug("First log")
    mainWindow.initialize();

    // Connect signal&slot
    QObject::connect(&mainWindow, SIGNAL(quit()), &a, SLOT(quit()));


    return a.exec();
}


