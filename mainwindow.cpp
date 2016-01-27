#include "mainwindow.h"
#include "ui_mainwindow.h"

mainWindow::mainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    _ui( new Ui::mainWindow ),
    _clockTime( QTime( 0, 0, 0 ) ),
    _running( false )
{
    _ui->setupUi( this );

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
    connect( _ui->act_addUiCharWindows, SIGNAL( triggered() ), this,
             SLOT( addUiCharWindows() ) );
    connect( _ui->act_log, SIGNAL( triggered( bool ) ), this,
             SLOT( setLog() ) );
}

void mainWindow::connectButtons()
{
    connect( _ui->btn_startMeasurement, SIGNAL( clicked() ), this,
             SLOT( startMeasurement() ) );
    connect( _ui->btn_stopMeasurement, SIGNAL( clicked() ), this,
             SLOT( stopMeasurement() ) );

    connect( _ui->btn_addWindow, SIGNAL( clicked() ), this,
             SLOT( addWindow() ) );
    connect( _ui->btn_addUiCharWindows, SIGNAL( clicked() ), this,
             SLOT( addUiCharWindows() ) );
}

void mainWindow::connectTimers()
{
    connect( &_updateTimer, SIGNAL( timeout() ), this, SLOT( doUpdate() ) );
    connect( &_clockTimer, SIGNAL( timeout() ), this, SLOT( timeUpdate() ) );
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

void mainWindow::addUiCharWindows()
{
    if( _mdi->getWindowNames().isEmpty() )
    {
        _mdi->addUiCharWindows();
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

void mainWindow::setLog()
{
    el::Configurations config;
    config.setToDefault();
    std::string enabled( "* GLOBAL:\n"
                            " FORMAT = %datetime{%Y-%M-%d %H:%m:%s.%g} %level "
                            "in %func: %msg\n"
                            " FILENAME = log.txt\n"
                            " ENABLED = true\n"
                            " TO_FILE = true\n"
                            " TO_STANDARD_OUTPUT = false\n"
                            " MILLISECONDS_WIDTH = 3\n"
                            " PERFORMANCE_TRACKING = true\n"
                            " MAX_LOG_FILE_SIZE = 67108864\n"
                            " LOG_FLUSH_THRESHOLD = 256" );
    std::string disabled( "*GLOBAL:\n ENABLED = false\n TO_FILE = false\n "
                            "TO_STANDARD_OUTPUT = false" );
    config.parseFromText( _ui->act_log->isChecked() ? enabled : disabled );
    el::Loggers::reconfigureAllLoggers( config );
    LOG(INFO) << "Logger enabled, "
              << _statusBarNumWindows->text().toStdString();
}

void mainWindow::exit()
{
    qApp->exit();
}
