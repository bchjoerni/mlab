#include "ms8050window.h"
#include "ui_ms8050window.h"

ms8050Window::ms8050Window( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::ms8050Window )
{
    _ui->setupUi( this );

    _unitStr[0] = "V AC";
    _unitStr[1] = "dBm";
    _unitStr[2] = "V DC";
    _unitStr[3] = "V AC+DC";
    _unitStr[4] = "mV DC";
    _unitStr[5] = "mV AC";
    _unitStr[6] = "V AC+DC";
    _unitStr[7] = "Hz";
    _unitStr[8] = "Duty";
    _unitStr[9] = "Ohm";
    _unitStr[10] = "Continuity";
    _unitStr[11] = "Capacity";
    _unitStr[12] = "uA DC";
    _unitStr[13] = "uA AC";
    _unitStr[14] = "uA AC+DC";
    _unitStr[15] = "mA DC";
    _unitStr[16] = "mA AC";
    _unitStr[17] = "mA AC+DC";
    _unitStr[18] = "A DC";
    _unitStr[19] = "A AC";
    _unitStr[20] = "A DC";

    connectPortFunctions();
    connectUiElements();
    refreshPortList();
}

ms8050Window::~ms8050Window()
{
    delete _ui;
}

void ms8050Window::connectPortFunctions()
{
    connect( &_port, SIGNAL( initSuccessful( QString ) ), this,
             SLOT( initFinished( QString ) ) );
    connect( &_port, SIGNAL( portError( QString ) ), this,
             SLOT( portError( QString ) ) );
    connect( &_port, SIGNAL( newValue( double, char ) ), this,
             SLOT( valueUpdate( double, char ) ) );
    connect( &_port, SIGNAL( newHold( bool ) ), this,
             SLOT( holdUpdate( bool ) ) );
    connect( &_port, SIGNAL( newMinMax( char ) ), this,
             SLOT( minMaxUpdate( char ) ) );
    connect( &_port, SIGNAL( newRelative( bool ) ), this,
             SLOT( relativeUpdate( bool ) ) );
}

void ms8050Window::connectUiElements()
{
    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_resetInfo, SIGNAL( clicked() ), this,
             SLOT( resetInfo() ) );
}

void ms8050Window::refreshPortList()
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

void ms8050Window::connectivityButtonPressed()
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

void ms8050Window::setPortEmits()
{
    _port.setEmitValue( true );
}

void ms8050Window::connectPort()
{
    setPortEmits();
    _port.openPort( _ui->cob_ports->currentText() );
}

void ms8050Window::disconnectPort()
{
    _port.closePort();

    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );

    emit changeWindowState( this->windowTitle(), false );
}

void ms8050Window::initFinished( const QString &idString )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": init finished, id: "
              << idString.toStdString();
    _ui->lbl_info->setText( idString );

    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );

    emit changeWindowState( this->windowTitle(), true );
}

void ms8050Window::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.updateValues();
    }
}

void ms8050Window::valueUpdate( double value, char unit )
{
    int index = static_cast<int>( unit );
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": " << value << " " << _unitStr[index].toStdString();
    LOG(INFO) << this->windowTitle().toStdString()
              << ": " << value << " " << _unitStr[index].toStdString();
    _ui->txt_value->setText( QString::number( value )
                                        + " " + _unitStr[index] );

    emit newValue( this->windowTitle() + ": " + _unitStr[index],
                   value );
}

void ms8050Window::holdUpdate( bool hold )
{
    _ui->lbl_hold->setVisible( hold );

    CLOG(INFO, "v") << this->windowTitle().toStdString() << ": hold=" << hold;
    LOG(INFO) << this->windowTitle().toStdString() << ": hold=" << hold;
}

void ms8050Window::minMaxUpdate( char minMax )
{
    if( minMax == 1 )
    {
        _ui->lbl_minMax->setVisible( true );
        _ui->lbl_minMax->setText( "MAX" );

        CLOG(INFO, "v") << this->windowTitle().toStdString() << ": MAX";
        LOG(INFO) << this->windowTitle().toStdString() << ": MAX";
    }
    else if( minMax == 2 )
    {
        _ui->lbl_minMax->setVisible( true );
        _ui->lbl_minMax->setText( "MIN" );

        CLOG(INFO, "v") << this->windowTitle().toStdString() << ": MIN";
        LOG(INFO) << this->windowTitle().toStdString() << ": MIN";
    }
    else if( minMax == 3 )
    {
        _ui->lbl_minMax->setVisible( true );
        _ui->lbl_minMax->setText( "MAX-MIN" );

        CLOG(INFO, "v") << this->windowTitle().toStdString() << ": MAX-MIN";
        LOG(INFO) << this->windowTitle().toStdString() << ": MAX-MIN";
    }
    else
    {
        _ui->lbl_minMax->setVisible( false );
    }
}

void ms8050Window::relativeUpdate( bool relative )
{
    _ui->lbl_relative->setVisible( relative );

    CLOG(INFO, "v") << this->windowTitle().toStdString() << ": rel="
                    << relative;
    LOG(INFO) << this->windowTitle().toStdString() << ": rel=" << relative;
}

void ms8050Window::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );

    emit changeWindowState( this->windowTitle(), false );
}

void ms8050Window::resetInfo()
{
    _port.clearPort();

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


