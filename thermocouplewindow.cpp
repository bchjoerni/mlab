#include "thermocouplewindow.h"
#include "ui_thermocouplewindow.h"

thermocoupleWindow::thermocoupleWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::thermocoupleWindow )
{
    _ui->setupUi( this );

    connectPortFunctions();
    connectUiElements();
    refreshPortList();
}

thermocoupleWindow::~thermocoupleWindow()
{
    delete _ui;
}

void thermocoupleWindow::connectPortFunctions()
{
    connect( &_port, SIGNAL( initSuccessful( QString ) ), this,
             SLOT( initFinished( QString ) ) );
    connect( &_port, SIGNAL( portError( QString ) ), this,
             SLOT( portError( QString ) ) );
    connect( &_port, SIGNAL( newAmbientTemperature( double ) ), this,
             SLOT( ambientTemperatureUpdate( double ) ) );
    connect( &_port, SIGNAL( newProbeTemperature( double ) ), this,
             SLOT( probeTemperatureUpdate( double ) ) );
}

void thermocoupleWindow::connectUiElements()
{
    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_resetInfo, SIGNAL( clicked() ), this,
             SLOT( resetInfo() ) );
}

void thermocoupleWindow::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );

            if( info.serialNumber().startsWith( "N160" ) )
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

void thermocoupleWindow::connectivityButtonPressed()
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

void thermocoupleWindow::setPortEmits()
{
    _port.setEmitProbeTemperature( true );
    _port.setEmitAmbientTemperature( true );
}

void thermocoupleWindow::connectPort()
{
    setPortEmits();
    _port.openPort( _ui->cob_ports->currentText() );
}

void thermocoupleWindow::disconnectPort()
{
    _port.closePort();

    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );

    emit changeWindowState( this->windowTitle(), false );
}

void thermocoupleWindow::initFinished( const QString &idString )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": init finished, id: "
              << idString.toStdString();
    _ui->lbl_info->setText( idString );

    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );

    emit changeWindowState( this->windowTitle(), true );
}

void thermocoupleWindow::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.updateValues();
    }
}

void thermocoupleWindow::probeTemperatureUpdate( double degreeCelsius )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": " << TEMPERATURE_PROBE.toStdString() << " = "
                    << degreeCelsius << " " << UNIT_CELSIUS.toStdString();
    LOG(INFO) << this->windowTitle().toStdString() << ": "
              << TEMPERATURE_PROBE.toStdString() << " : " << degreeCelsius;
    _ui->txt_probeTemperature->setText( QString::number( degreeCelsius )
                                        + " " + UNIT_CELSIUS );
    emit newValue( this->windowTitle() + ": " + TEMPERATURE_PROBE,
                   degreeCelsius );
}

void thermocoupleWindow::ambientTemperatureUpdate( double degreeCelsius )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": " << TEMPERATURE_AMBIENT.toStdString() << " = "
                    << degreeCelsius << " " << UNIT_CELSIUS.toStdString();
    LOG(INFO) << this->windowTitle().toStdString() << ": "
              << TEMPERATURE_AMBIENT.toStdString() << " : " << degreeCelsius;
    _ui->txt_ambientTemperature->setText( QString::number( degreeCelsius )
                                          + " " + UNIT_CELSIUS );
    emit newValue( this->windowTitle() + ": " + TEMPERATURE_AMBIENT,
                   degreeCelsius );
}

void thermocoupleWindow::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );

    emit newError( this->windowTitle() + ": " + error );
    emit changeWindowState( this->windowTitle(), false );
}

void thermocoupleWindow::resetInfo()
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
