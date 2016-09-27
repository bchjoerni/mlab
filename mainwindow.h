#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QTime>

#include <fstream>

#include "easylogging++.h"
#include "mdihandler.h"
#include "windowadderwindow.h"

namespace Ui
{
    class mainWindow;
}

class mainWindow : public QMainWindow
{
    Q_OBJECT

    const int CLOCK_INTERVAL_MS = 200;

public:
    explicit mainWindow( QWidget *parent = 0 );
    ~mainWindow();

public slots:
    void windowNumberChanged( int change );

private slots:
    void emergencyStop();
    void startMeasurement();
    void stopMeasurement();
    void tileWindows();
    void cascadeWindows();
    void addWindow();
    void addUICharWindowsOld();
    void addUICharWindowsNew();
    void doUpdate();
    void timeUpdate();
    void setValueLogger();
    void setInfoLogger();
    void exit();

private:
    Ui::mainWindow *_ui;
    mdiHandler* _mdi;
    QLabel* _statusBarNumWindows;
    QTimer _updateTimer;
    QTimer _clockTimer;
    QTime _clockTime;
    bool _running;
    bool _infoLogRunning;
    bool _valueLogRunning;
    bool _infoLoggerCleared;

    void connectActions();
    void connectButtons();
    void connectTimers();
    void setStartStopButtons( bool running );
};

#endif // MAINWINDOW_H
