#include "tpg26xwindow.h"
#include "ui_tpg26xwindow.h"

tpg26xWindow::tpg26xWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::tpg26xWindow )
{
    _ui->setupUi( this );

    connectPortFunctions();
    connectUiElements();
    refreshPortList();
}

tpg26xWindow::~tpg26xWindow()
{
    delete _ui;
}

void tpg26xWindow::connectPortFunctions()
{
    connect( &_port, SIGNAL( initSuccessful( QString ) ), this,
             SLOT( initFinished( QString ) ) );
    connect( &_port, SIGNAL( portError( QString ) ), this,
             SLOT( portError( QString ) ) );
    connect( &_port, SIGNAL( newPressureUnit( QString ) ), this,
             SLOT( unitUpdate( QString ) ) );
    connect( &_port, SIGNAL( newPressureGauge1( double ) ), this,
             SLOT( pressureGauge1Update( double ) ) );
    connect( &_port, SIGNAL( newPressureGauge2( double ) ), this,
             SLOT( pressureGauge2Update( double ) ) );
}

void tpg26xWindow::connectUiElements()
{
    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_resetInfo, SIGNAL( clicked() ), this,
             SLOT( resetInfo() ) );
}

void tpg26xWindow::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );

            if( info.serialNumber().startsWith( "FTVA" ) )
            {
                _ui->cob_ports->setCurrentText( info.portName() );
            }
        }
    }

    _ui->btn_connect->setEnabled( _ui->cob_ports->count() > 0 );
    _ui->cob_ports->setEnabled( _ui->cob_ports->count() > 0 );

    if( _ui->cob_ports->count() < 1 )
    {
        _ui->cob_ports->addItem( NO_PORTS_AVAILABLE );
    }
}

void tpg26xWindow::connectivityButtonPressed()
{
    if( _ui->btn_connect->text() == CONNECT_PORT )
    {
        connectPort();
    }
    else if( _ui->btn_connect->text() == DISCONNECT_PORT )
    {
        disconnectPort();
    }
}

void tpg26xWindow::setPortEmits()
{
    _port.setEmitGauge1( true );
    _port.setEmitGauge2( true );
}

void tpg26xWindow::connectPort()
{
    setPortEmits();
    _port.openPort( _ui->cob_ports->currentText() );
}

void tpg26xWindow::disconnectPort()
{
    _port.closePort();

    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );

    emit changeWindowState( this->windowTitle(), false );
}

void tpg26xWindow::initFinished( const QString &idString )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": init finished, id: "
              << idString.toStdString();
    _ui->lbl_info->setText( idString );

    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );

    emit changeWindowState( this->windowTitle(), true );
}

void tpg26xWindow::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.updateValues();
    }
}

void tpg26xWindow::unitUpdate( const QString &unit )
{
    _unit = unit;
}

void tpg26xWindow::pressureGauge1Update( double mbarPressure )
{
    if( mbarPressure >= 0.0 )
    {
        CLOG(INFO, "v") << this->windowTitle().toStdString()
                        << ": " << PRESSURE_GAUGE_1.toStdString() << " = "
                        << mbarPressure << " " << _unit.toStdString();
        LOG(INFO) << this->windowTitle().toStdString() << ": "
                  << PRESSURE_GAUGE_1.toStdString() << " : " << mbarPressure;
        _ui->txt_gauge1->setText( QString::number( mbarPressure )
                                            + " " + _unit );
        emit newValue( this->windowTitle() + ": " + PRESSURE_GAUGE_1 + " ["
                       + _unit + "]", mbarPressure );
    }
    else
    {
        int errorNum = -static_cast<int>( mbarPressure );
        if( errorNum <= 0 || errorNum >= 8 )
        {
            errorNum = 7;
        }
        CLOG(INFO, "v") << this->windowTitle().toStdString()
                        << ": " << PRESSURE_GAUGE_1.toStdString() << ": "
                        << PR_ERRORS[errorNum].toStdString();
        LOG(INFO) << this->windowTitle().toStdString() << ": "
                  << PRESSURE_GAUGE_1.toStdString() << " : "
                  << PR_ERRORS[errorNum].toStdString();
        _ui->txt_gauge1->setText( PR_ERRORS[errorNum] );
    }
}

void tpg26xWindow::pressureGauge2Update( double mbarPressure )
{
    if( mbarPressure >= 0.0 )
    {
        CLOG(INFO, "v") << this->windowTitle().toStdString()
                        << ": " << PRESSURE_GAUGE_2.toStdString() << " = "
                        << mbarPressure << " " << _unit.toStdString();
        LOG(INFO) << this->windowTitle().toStdString() << ": "
                  << PRESSURE_GAUGE_2.toStdString() << " : " << mbarPressure;
        _ui->txt_gauge2->setText( QString::number( mbarPressure )
                                            + " " + _unit );
        emit newValue( this->windowTitle() + ": " + PRESSURE_GAUGE_2 + " ["
                       + _unit + "]", mbarPressure );
    }
    else
    {
        int errorNum = -static_cast<int>( mbarPressure );
        if( errorNum <= 0 || errorNum >= 8 )
        {
            errorNum = 7;
        }
        CLOG(INFO, "v") << this->windowTitle().toStdString()
                        << ": " << PRESSURE_GAUGE_2.toStdString() << ": "
                        << PR_ERRORS[errorNum].toStdString();
        LOG(INFO) << this->windowTitle().toStdString() << ": "
                  << PRESSURE_GAUGE_2.toStdString() << " : "
                  << PR_ERRORS[errorNum].toStdString();
        _ui->txt_gauge2->setText( PR_ERRORS[errorNum] );
    }
}

void tpg26xWindow::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );

    emit changeWindowState( this->windowTitle(), false );
}

void tpg26xWindow::resetInfo()
{
    _port.clearPortErrors();

    if( _port.isRunning() )
    {
        _ui->lbl_info->setText( _port.idString() );
        _ui->lbl_info->setStyleSheet( "" );
        _ui->btn_connect->setText( DISCONNECT_PORT );
        _ui->btn_connect->setEnabled( true );

        emit changeWindowState( this->windowTitle(), true );
    }
    else
    {
        _ui->lbl_info->setText( "-" );
        _ui->lbl_info->setStyleSheet( "" );
        _ui->btn_connect->setText( CONNECT_PORT );
        refreshPortList();
    }
}
