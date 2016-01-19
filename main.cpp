#define ELPP_NO_DEFAULT_LOG_FILE
#include "easylogging++.h"

#include "mainwindow.h"
#include <QApplication>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    el::Configurations config;
    config.setToDefault();
    config.parseFromText( "*GLOBAL:\n ENABLED = false\n TO_FILE = false\n "
                          "TO_STANDARD_OUTPUT = false" );
    el::Loggers::reconfigureAllLoggers( config );

    QApplication a(argc, argv);
    mainWindow w;
    w.show();

    return a.exec();
}
