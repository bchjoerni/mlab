#include "keithley2400window.h"
#include "ui_keithley2400window.h"


keithley2400Window::keithley2400Window( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::keithley2400Window )
{
    _ui->setupUi( this );

    connectPortFunctions();
    connectUiElements();
    refreshPortList();
}

keithley2400Window::~keithley2400Window()
{
    delete _ui;
}

void keithley2400Window::connectPortFunctions()
{
    connect( &_port, SIGNAL( initSuccessful( QString ) ), this,
             SLOT( initFinished( QString ) ) );
    connect( &_port, SIGNAL( portError( QString ) ), this,
             SLOT( portError( QString ) ) );
    connect( &_port, SIGNAL( newResistance( double ) ), this,
             SLOT( resistanceUpdate( double ) ) );
}

void keithley2400Window::connectUiElements()
{
    connect( _ui->btn_emergencyStop, SIGNAL( clicked() ), this,
             SLOT( emergencyStop() ) );

    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_resetInfo, SIGNAL( clicked() ), this,
             SLOT( resetInfo() ) );
}

void keithley2400Window::setPortEmits()
{
    _port.setEmitResistance( _ui->frame_resistance->isVisible() );
}

void keithley2400Window::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );

            if( info.serialNumber() == "TODO" )
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

void keithley2400Window::mLabSignal( char signal, const QString& cmd )
{
    if( signal == SIGNAL_SHUTDOWN )
    {
        emergencyStop();
    }
    else if( signal == SIGNAL_STOP && _port.isOpen() )
    {
        emergencyStop();
    }
}

void keithley2400Window::emergencyStop()
{
    if( _port.isOpen() )
    {
        _ui->lbl_info->setText( EMERGENCY_STOP );
        _ui->lbl_info->setStyleSheet( STYLE_ERROR );
        emit changeWindowState( this->windowTitle(), false );
    }
}

void keithley2400Window::connectivityButtonPressed()
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

void keithley2400Window::connectPort()
{
    setPortEmits();
    _port.openPort( _ui->cob_ports->currentText() );
}

void keithley2400Window::disconnectPort()
{
    _port.closePort();

    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );

    emit changeWindowState( this->windowTitle(), false );
}

void keithley2400Window::initFinished( const QString &idString )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": init finished, id: "
              << idString.toStdString();
    _ui->lbl_info->setText( idString );

    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );

    emit changeWindowState( this->windowTitle(), true );
}

void keithley2400Window::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.updateValues();
    }
}

void keithley2400Window::resistanceUpdate( double resistance )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": resistance = " << resistance << " Ohm";
    LOG(INFO) << this->windowTitle().toStdString() << ": resistance update: "
              << resistance;
    _ui->txt_resistance->setText( QString::number( resistance )
                                  + " " + UNIT_OHM );

    if( _ui->chb_shareResistance->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + RESISTANCE, resistance );
    }
}

void keithley2400Window::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );

    emit newError( this->windowTitle() + ": " + error );
    emit changeWindowState( this->windowTitle(), false );
}

void keithley2400Window::resetInfo()
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
