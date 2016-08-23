#include "tsh071window.h"
#include "ui_tsh071window.h"

tsh071Window::tsh071Window( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::tsh071Window )
{
    _ui->setupUi( this );

    connectPortFunctions();
    connectUiElements();

    _ui->frame_pressure->setVisible( false );
    _ui->frame_setRpm->setVisible( false ); // READ ONLY value - cannot be set!!

    addItems();
    refreshPortList();
    visibilitySelectionChanged();
}

tsh071Window::~tsh071Window()
{
    delete _ui;
}

void tsh071Window::connectPortFunctions()
{
    connect( &_port, SIGNAL( initSuccessful( QString ) ), this,
             SLOT( initFinished( QString ) ) );
    connect( &_port, SIGNAL( portError( QString ) ), this,
             SLOT( portError( QString ) ) );

    connect( &_port, SIGNAL( newRpm( int ) ), this, SLOT( rpmUpdate( int ) ) );
    connect( &_port, SIGNAL( newPressure( double ) ), this,
             SLOT( pressureUpdate( double ) ) );
}

void tsh071Window::connectUiElements()
{
    connect( _ui->cob_measuredValues, SIGNAL( currentTextChanged( QString ) ),
             this, SLOT( visibilitySelectionChanged() ) );

    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_setRpm, SIGNAL( clicked() ), this,
             SLOT( setRotation() ) );
    connect( _ui->btn_measuredValuesVisibility, SIGNAL( clicked() ), this,
             SLOT( changeVisibility() ) );
    connect( _ui->btn_resetInfo, SIGNAL( clicked() ), this,
             SLOT( resetInfo() ) );
    connect( _ui->btn_startPump, SIGNAL( clicked() ), this,
             SLOT( startPump() ) );
    connect( _ui->btn_stopPump, SIGNAL( clicked() ), this,
             SLOT( stopPump() ) );
}

void tsh071Window::addItems()
{
    _ui->cob_measuredValues->addItem( ROTATION );
    _ui->cob_measuredValues->addItem( PRESSURE );
}

void tsh071Window::setPortEmits()
{
    _port.setEmitRpm(      _ui->frame_rpm->isVisible() );
    _port.setEmitPressure( _ui->frame_pressure->isVisible() );
}

void tsh071Window::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );

            if( info.serialNumber() == "FTYIWRSL" )
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

void tsh071Window::connectivityButtonPressed()
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

void tsh071Window::connectPort()
{
    setPortEmits();
    _port.setId( _ui->spb_id->value() );
    _port.openPort( _ui->cob_ports->currentText() );
}

void tsh071Window::disconnectPort()
{
    _port.closePort();

    _ui->btn_setRpm->setEnabled( false );
    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );
    _ui->spb_id->setEnabled( true );

    emit changeWindowState( this->windowTitle(), false );
}

void tsh071Window::initFinished( const QString &idString )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": init finished, id: "
              << idString.toStdString();
    _ui->lbl_info->setText( idString );

    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );
    _ui->spb_id->setEnabled( false );
}

void tsh071Window::setRotation()
{
    double value = static_cast<double>( _ui->spb_rpm->value() );

    _port.setValue( tsh071Port::setValueType::setTypeRpm, value );
    _ui->lbl_rpmSet->setText( QString::number( value ) );
}

void tsh071Window::startPump()
{
    _port.startPump();
    _ui->lbl_running->setText( "running" );
    _ui->lbl_running->setStyleSheet( STYLE_OK );

    emit changeWindowState( this->windowTitle(), true );
}

void tsh071Window::stopPump()
{
    _port.stopPump();
    _ui->lbl_running->setText( "pausing" );
    _ui->lbl_running->setStyleSheet( STYLE_ERROR );

    emit changeWindowState( this->windowTitle(), false );
}

void tsh071Window::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.updateValues();
    }
}

void tsh071Window::rpmUpdate( int rpm )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": rpm = " << rpm << " /min";
    LOG(INFO) << this->windowTitle().toStdString() << ": rpm update: "
              << rpm;
    _ui->txt_rpm->setText( QString::number( rpm ) + " / min" );

    if( _ui->chb_shareRpm->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + ROTATION,
                       static_cast<double>( rpm ) );
    }
}

void tsh071Window::pressureUpdate( double pressure )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
               << ": pressure = " << pressure << " mbar";
    LOG(INFO) << this->windowTitle().toStdString() << ": pressure update: "
              << pressure;
    _ui->txt_pressure->setText( QString::number( pressure ) + " mbar" );

    if( _ui->chb_sharePressure->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + PRESSURE, pressure );
    }
}

void tsh071Window::visibilitySelectionChanged()
{
    QString text = _ui->cob_measuredValues->currentText();

    if( text == ROTATION )
    {
        if( _ui->frame_rpm->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
    else if( text == PRESSURE )
    {
        if( _ui->frame_pressure->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
}

void tsh071Window::changeVisibility()
{
    QString text = _ui->cob_measuredValues->currentText();

    if( text == ROTATION )
    {
        _ui->frame_rpm->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_shareRpm->setChecked( false );
    }
    else if( text == PRESSURE )
    {
        _ui->frame_pressure->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_sharePressure->setChecked( false );
    }
    visibilitySelectionChanged();
    setPortEmits();
}

void tsh071Window::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );

    emit changeWindowState( this->windowTitle(), false );
}

void tsh071Window::resetInfo()
{
    _port.clearPortErrors();

    if( _port.isRunning() )
    {
        _ui->lbl_info->setText( _port.idString() );
        _ui->lbl_info->setStyleSheet( "" );
        _ui->btn_connect->setText( DISCONNECT_PORT );
        _ui->btn_connect->setEnabled( true );
        _ui->spb_id->setEnabled( false );

        emit changeWindowState( this->windowTitle(), true );
    }
    else
    {
        _ui->lbl_info->setText( "-" );
        _ui->lbl_info->setStyleSheet( "" );
        _ui->btn_connect->setText( CONNECT_PORT );
        _ui->spb_id->setEnabled( true );
        refreshPortList();
    }
}
