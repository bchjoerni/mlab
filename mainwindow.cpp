#include "mainwindow.h"
#include "ui_mainwindow.h"

mainWindow::mainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    _ui( new Ui::mainWindow ),
    _clockTime( QTime( 0, 0, 0 ) ),
    _running( false ), _infoLogRunning( false ), _valueLogRunning( true ),
    _infoLoggerCleared( false )
{
    _ui->setupUi( this );
    _ui->act_logInfos->setChecked( _infoLogRunning );
    _ui->act_logValues->setChecked( _valueLogRunning );

    _statusBarNumWindows = new QLabel;
    _ui->statusBar->addPermanentWidget( _statusBarNumWindows );
    _statusBarNumWindows->setText( "Windows: 0" );
    _mdi = new mdiHandler( _ui->mdiArea, this );

    connect( _mdi, SIGNAL( windowNumberChanged( int ) ), this,
             SLOT( windowNumberChanged( int ) ) );

    connectActions();
    connectButtons();
    connectTimers();
}

mainWindow::~mainWindow()
{
    delete _statusBarNumWindows;
    _statusBarNumWindows = nullptr;

    delete _ui;
}

void mainWindow::connectActions()
{
    connect( _ui->act_exit, SIGNAL( triggered() ), this, SLOT( exit() ) );
    connect( _ui->act_addWindow, SIGNAL( triggered() ), this,
             SLOT( addWindow() ) );
    connect( _ui->act_addUiCharWindows_old, SIGNAL( triggered() ), this,
             SLOT( addUICharWindowsOld() ) );
    connect( _ui->act_addUICharWindows_new, SIGNAL( triggered() ), this,
             SLOT( addUICharWindowsNew() ) );
    connect( _ui->act_logInfos, SIGNAL( triggered( bool ) ), this,
             SLOT( setInfoLogger() ) );
    connect( _ui->act_logValues, SIGNAL( triggered( bool ) ), this,
             SLOT( setValueLogger() ) );
}

void mainWindow::connectButtons()
{
    connect( _ui->btn_emergencyStop, SIGNAL( clicked() ), this,
             SLOT( emergencyStop() ) );
    connect( _ui->btn_startMeasurement, SIGNAL( clicked() ), this,
             SLOT( startMeasurement() ) );
    connect( _ui->btn_stopMeasurement, SIGNAL( clicked() ), this,
             SLOT( stopMeasurement() ) );

    connect( _ui->btn_addWindow, SIGNAL( clicked() ), this,
             SLOT( addWindow() ) );
    connect( _ui->btn_addUiCharWindows_new, SIGNAL( clicked() ), this,
             SLOT( addUICharWindowsNew() ) );
}

void mainWindow::connectTimers()
{
    connect( &_updateTimer, SIGNAL( timeout() ), this, SLOT( doUpdate() ) );
    connect( &_clockTimer, SIGNAL( timeout() ), this, SLOT( timeUpdate() ) );
}

void mainWindow::emergencyStop()
{
    _mdi->emergencyStop();
    QMessageBox::critical( this, "Emergency stop", "Emergency stop pressed. "
                                                   "The program tries to stop "
                                                   "all devices." );
}

void mainWindow::startMeasurement()
{
    setStartStopButtons( true );
    _updateTimer.start( static_cast<int>( _ui->dsp_updateInterval->value() *
                                          1000 ) );
    _clockTimer.start( CLOCK_INTERVAL_MS );
    _ui->lbl_updateStatus->setText( " running " );
    _ui->lbl_updateStatus->setStyleSheet( "color: green" );
    LOG(INFO) << "update timer started, interval: "
              << _ui->dsp_updateInterval->value() << " s";
}

void mainWindow::stopMeasurement()
{
    setStartStopButtons( false );
    _updateTimer.stop();
    _clockTimer.stop();
    _ui->lbl_updateStatus->setText( " paused  " );
    _ui->lbl_updateStatus->setStyleSheet( "color: red" );
    LOG(INFO) << "update timer stopped";
}

void mainWindow::addWindow()
{
    windowAdderWindow waw( _mdi, this );
    waw.exec();
}

void mainWindow::addUICharWindowsNew()
{
    if( _mdi->getWindowNames().isEmpty() )
    {
        LOG(INFO) << "add UI-Characterization windows (new)";
        _mdi->addUICharWindowsNew();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle( "Error!" );
        msgBox.setText( "UI Windows can only be added if there are no other "
                        "windows!" );
        msgBox.exec();
    }
}

void mainWindow::addUICharWindowsOld()
{
    if( _mdi->getWindowNames().isEmpty() )
    {
        LOG(INFO) << "add UI-Characterization windows (old)";
        _mdi->addUICharWindowsOld();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle( "Error!" );
        msgBox.setText( "UI Windows can only be added if there are no other "
                        "windows!" );
        msgBox.exec();
    }
}

void mainWindow::windowNumberChanged( int change )
{
    bool valid = false;
    int numWindows = _statusBarNumWindows->text().mid(
                _statusBarNumWindows->text().indexOf( ":" )+2 ).toInt( &valid );
    if( valid )
    {
        numWindows += change;
        if( numWindows >= 0 )
        {
            _statusBarNumWindows->setText( "Windows: " +
                                           QString::number( numWindows ) );
        }
        else
        {
            _statusBarNumWindows->setText( "Windows: 0" );
        }
    }
    else
    {
        _statusBarNumWindows->setText( "Windows: ?" );
    }
}

void mainWindow::setStartStopButtons( bool running )
{
    _ui->btn_startMeasurement->setEnabled( !running );
    _ui->btn_stopMeasurement->setEnabled( running );
}

void mainWindow::doUpdate()
{
    LOG(INFO) << "do update ...";
    _mdi->doUpdates();
}

void mainWindow::timeUpdate()
{
    _clockTime = _clockTime.addMSecs( CLOCK_INTERVAL_MS );
    _ui->time_timeRunning->setTime( _clockTime );
}

void mainWindow::setValueLogger()
{
    if( _valueLogRunning == _ui->act_logValues->isChecked() )
    {
        return;
    }
    else
    {
        _valueLogRunning = _ui->act_logValues->isChecked();
    }

    if( !_ui->act_logValues->isChecked() )
    {
        CLOG(INFO, "v") << "logger disabled";
    }
    el::Configurations config;
    config.setToDefault();
    std::string enabled( "*GLOBAL:\n"
                          " FORMAT = %datetime{%H:%m:%s.%g}: %msg\n"
                          " FILENAME = log_values.txt\n"
                          " ENABLED = true\n"
                          " TO_FILE = true\n"
                          " TO_STANDARD_OUTPUT = false\n"
                          " MILLISECONDS_WIDTH = 3\n"
                          " MAX_LOG_FILE_SIZE = 67108864\n" );
    std::string disabled( "*GLOBAL:\n ENABLED = false\n TO_FILE = false\n "
                            "TO_STANDARD_OUTPUT = false" );
    config.parseFromText( _ui->act_logInfos->isChecked() ? enabled : disabled );
    el::Loggers::reconfigureLogger( "v", config );
    CLOG(INFO, "v") << "logger enabled";
}

void mainWindow::setInfoLogger()
{
    if( _infoLogRunning == _ui->act_logInfos->isChecked() )
    {
        return;
    }
    else
    {
        _infoLogRunning = _ui->act_logInfos->isChecked();
    }
    if( _infoLogRunning && !_infoLoggerCleared )
    {
        std::fstream logFile;
        logFile.open( "log_info.txt", std::ios_base::out );
        logFile.close();
        _infoLoggerCleared = true;
    }

    el::Configurations config;
    config.setToDefault();
    std::string enabled( "*GLOBAL:\n"
                            " FORMAT = %datetime{%Y-%M-%d %H:%m:%s.%g} %level "
                            "in %func: %msg\n"
                            " FILENAME = log_info.txt\n"
                            " ENABLED = true\n"
                            " TO_FILE = true\n"
                            " TO_STANDARD_OUTPUT = false\n"
                            " MILLISECONDS_WIDTH = 3\n"
                            " MAX_LOG_FILE_SIZE = 67108864\n" );
    std::string disabled( "*GLOBAL:\n ENABLED = false\n TO_FILE = false\n "
                            "TO_STANDARD_OUTPUT = false" );
    config.parseFromText( _ui->act_logInfos->isChecked() ? enabled : disabled );
    el::Loggers::reconfigureLogger( "default", config );
    LOG(INFO) << "Logger enabled, "
              << _statusBarNumWindows->text().toStdString();
}

void mainWindow::exit()
{
    qApp->exit();
}
