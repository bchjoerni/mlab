#define ELPP_NO_DEFAULT_LOG_FILE
#define ELPP_DISABLE_LOGS
#include "easylogging++.h"

#include "mainwindow.h"
#include <QApplication>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainWindow w;
    w.show();

    return a.exec();
}
