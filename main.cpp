#define ELPP_NO_DEFAULT_LOG_FILE
#include "easylogging++.h"

#include "mainwindow.h"

#include <QApplication>
#include <QDateTime>

#include <fstream>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    el::Configurations config;
    config.setToDefault();
    config.parseFromText( "*GLOBAL:\n ENABLED = false\n TO_FILE = false\n "
                          "TO_STANDARD_OUTPUT = false" );
    el::Loggers::reconfigureAllLoggers( config );

    std::fstream logFile;
    logFile.open( "log_values.txt", std::ios_base::out );
    logFile.close();
    config.parseFromText( "*GLOBAL:\n"
                          " FORMAT = %datetime{%H:%m:%s.%g}: %msg\n"
                          " FILENAME = log_values.txt\n"
                          " ENABLED = true\n"
                          " TO_FILE = true\n"
                          " TO_STANDARD_OUTPUT = false\n"
                          " MILLISECONDS_WIDTH = 3\n"
                          " MAX_LOG_FILE_SIZE = 67108864\n" );
    el::Loggers::reconfigureLogger( "v", config );
    CLOG(INFO, "v") << "value logger started at " <<
                       QDateTime::currentDateTime().toString(
                           "yyyy-MM-dd hh:mm:ss" ).toStdString();

    QApplication a(argc, argv);
    mainWindow w;
    w.showMaximized();
    w.show();

    return a.exec();
}
