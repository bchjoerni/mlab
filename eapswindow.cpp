#include "eapswindow.h"
#include "ui_eapswindow.h"

eapsWindow::eapsWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::eapsWindow )
{
    _ui->setupUi( this );

    connect( &_port, SIGNAL( initSuccessful( QString ) ), this,
             SLOT( initFinished( QString ) ) );
    connect( &_port, SIGNAL( portError( QString ) ), this,
             SLOT( portError( QString ) ) );
    connect( &_port, SIGNAL( newVoltage( double ) ), this,
             SLOT( voltageUpdate( double ) ) );
    connect( &_port, SIGNAL( newCurrent( double ) ), this,
             SLOT( currentUpdate( double ) ) );
    connect( &_port, SIGNAL( newPower( double ) ), this,
             SLOT( powerUpdate( double ) ) );
    connect( _ui->cob_measuredValues, SIGNAL( currentTextChanged( QString ) ),
             this, SLOT( visibilitySelectionChanged() ) );
    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_setValue, SIGNAL( clicked() ), this, SLOT( setValue() ) );
    connect( _ui->btn_measuredValuesVisibility, SIGNAL( clicked() ), this,
             SLOT( changeVisibility() ) );
    connect( _ui->cob_setValue, SIGNAL( currentTextChanged( QString ) ), this,
             SLOT( setValueSelectionChanged() ) );
    connect( _ui->btn_resetStatus, SIGNAL( clicked() ), this,
             SLOT( resetInfoLabel() ) );

    addItems();
    refreshPortList();
    visibilitySelectionChanged();
}

eapsWindow::~eapsWindow()
{
    delete _ui;
}

void eapsWindow::addItems()
{
    _ui->cob_setValue->addItem( VOLTAGE );
    _ui->cob_setValue->addItem( CURRENT );
    // _ui->cob_setValue->addItem( POWER   );

    _ui->cob_measuredValues->addItem( VOLTAGE );
    _ui->cob_measuredValues->addItem( CURRENT );
    _ui->cob_measuredValues->addItem( POWER   );
}

void eapsWindow::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        _ui->cob_ports->addItem( info.portName() );
    }

    _ui->btn_connect->setEnabled( _ui->cob_ports->count() > 0 );
    _ui->cob_ports->setEnabled( _ui->cob_ports->count() > 0 );

    if( _ui->cob_ports->count() < 1 )
    {
        _ui->cob_ports->addItem( NO_PORTS_AVAILABLE );
    }
}

void eapsWindow::connectivityButtonPressed()
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

void eapsWindow::connectPort()
{
    _port.open( _ui->cob_ports->currentText() );
}

void eapsWindow::disconnectPort()
{
    _port.closePort();

    _ui->btn_setValue->setEnabled( false );
    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );
}

void eapsWindow::initFinished( const QString &idString )
{
    _ui->lbl_info->setText( idString );

    _ui->btn_setValue->setEnabled( true );
    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );
}

void eapsWindow::setValue()
{
    double value = _ui->dsp_setValue->value();

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIVOLTS )
        {
            value /= 1000.0;
        }
        _port.setValue( eapsPort::eapsSetValues::voltage, value,
                        _ui->chb_adjustSetValue->isChecked() );
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIAMPERE )
        {
            value /= 1000.0;
        }
        _port.setValue( eapsPort::eapsSetValues::current, value,
                        _ui->chb_adjustSetValue->isChecked() );
    }

    _ui->lbl_setValueSet->setText( "value set" );
}

void eapsWindow::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.getUpdateValues();
    }
}

void eapsWindow::voltageUpdate( double voltage )
{
    _ui->txt_voltage->setText( QString::number( voltage ) + " V" );
    emit newValue( this->windowTitle() + ": " + VOLTAGE, voltage );
}

void eapsWindow::currentUpdate( double current )
{
    _ui->txt_current->setText( QString::number( current ) + " A" );
    emit newValue( this->windowTitle() + ": " + CURRENT, current );
}

void eapsWindow::powerUpdate( double power )
{
    _ui->txt_power->setText( QString::number( power ) + " W" );
    emit newValue( this->windowTitle() + ": " + POWER, power );
}

void eapsWindow::visibilitySelectionChanged()
{
    QString text = _ui->cob_measuredValues->currentText();

    if( text == VOLTAGE )
    {
        if( _ui->frame_voltage->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
    else if( text == CURRENT )
    {
        if( _ui->frame_current->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
    else if( text == POWER )
    {
        if( _ui->frame_power->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
}

void eapsWindow::changeVisibility()
{
    QString text = _ui->cob_measuredValues->currentText();

    if( text == VOLTAGE )
    {
        _ui->frame_voltage->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_shareVoltage->setChecked( false );
    }
    else if( text == CURRENT )
    {
        _ui->frame_current->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_shareCurrent->setChecked( false );
    }
    else if( text == POWER )
    {
        _ui->frame_power->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_sharePower->setChecked( false );
    }
    visibilitySelectionChanged();
}

void eapsWindow::setValueSelectionChanged()
{
    _ui->cob_setValueUnit->clear();
    _ui->lbl_setValueSet->setText( "not set" );

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        _ui->cob_setValueUnit->addItem( UNIT_VOLTS );
        _ui->cob_setValueUnit->addItem( UNIT_MILLIVOLTS );
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        _ui->cob_setValueUnit->addItem( UNIT_AMPERE );
        _ui->cob_setValueUnit->addItem( UNIT_MILLIAMPERE );
    }
}

void eapsWindow::portError( QString error )
{
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );
}

void eapsWindow::resetInfoLabel()
{
    _ui->lbl_info->setText( "-" );
    _ui->lbl_info->setStyleSheet( "" );
}

