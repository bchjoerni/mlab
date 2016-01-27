#include "eapswindow.h"
#include "ui_eapswindow.h"

eapsWindow::eapsWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::eapsWindow )
{
    _ui->setupUi( this );

    connectPortFunctions();
    connectUiElements();

    addItems();
    refreshPortList();
    visibilitySelectionChanged();

    setPortEmits();
}

eapsWindow::~eapsWindow()
{
    delete _ui;
}

void eapsWindow::connectPortFunctions()
{
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
    connect( &_port, SIGNAL( newResistance( double ) ), this,
             SLOT( resistanceUpdate( double ) ) );
}

void eapsWindow::connectUiElements()
{
    connect( _ui->cob_measuredValues, SIGNAL( currentTextChanged( QString ) ),
             this, SLOT( visibilitySelectionChanged() ) );
    connect( _ui->cob_setValue, SIGNAL( currentTextChanged( QString ) ), this,
             SLOT( setValueSelectionChanged() ) );
    connect( _ui->cob_setValueUnit, SIGNAL( currentTextChanged( QString ) ),
             this, SLOT( updateUnitRange() ) );

    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_setValue, SIGNAL( clicked() ), this, SLOT( setValue() ) );
    connect( _ui->btn_measuredValuesVisibility, SIGNAL( clicked() ), this,
             SLOT( changeVisibility() ) );
    connect( _ui->btn_resetInfo, SIGNAL( clicked() ), this,
             SLOT( resetInfo() ) );
}

void eapsWindow::addItems()
{
    _ui->cob_setValue->addItem( VOLTAGE );
    _ui->cob_setValue->addItem( CURRENT );
    _ui->cob_setValue->addItem( POWER_BY_VOLTAGE );
    _ui->cob_setValue->addItem( POWER_BY_CURRENT );
    _ui->cob_setValue->addItem( RESISTANCE_BY_VOLTAGE );
    _ui->cob_setValue->addItem( RESISTANCE_BY_CURRENT );

    _ui->cob_measuredValues->addItem( VOLTAGE );
    _ui->cob_measuredValues->addItem( CURRENT );
    _ui->cob_measuredValues->addItem( POWER );
    _ui->cob_measuredValues->addItem( RESISTANCE );
}

void eapsWindow::setPortEmits()
{
    _port.setEmitVoltage(    _ui->frame_voltage->isVisible() );
    _port.setEmitCurrent(    _ui->frame_current->isVisible() );
    _port.setEmitPower(      _ui->frame_power->isVisible() );
    _port.setEmitResistance( _ui->frame_resistance->isVisible() );
}

void eapsWindow::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );

            if( info.serialNumber() == "EAVI20R5" )
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
    _port.openPort( _ui->cob_ports->currentText() );
}

void eapsWindow::disconnectPort()
{
    _port.closePort();

    _ui->btn_setValue->setEnabled( false );
    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );
    _ui->lbl_setValueSet->setText( "-" );

    emit changeWindowState( this->windowTitle(), false );
}

void eapsWindow::initFinished( const QString &idString )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": init finished, id: "
              << idString.toStdString();
    _ui->lbl_info->setText( idString );

    _ui->btn_setValue->setEnabled( true );
    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );

    emit changeWindowState( this->windowTitle(), true );
    updateUnitRange();
}

void eapsWindow::setValue()
{
    double value = _ui->dsp_setValue->value();

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIVOLT )
        {
            value /= 1000.0;
        }
        _port.setValue( eapsPort::setValueType::setTypeVoltage, value,
                        _ui->chb_adjustSetValue->isChecked() );
        _ui->lbl_setValueSet->setText( "set to " + QString::number( value )
                                       + UNIT_VOLT );
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIAMPERE )
        {
            value /= 1000.0;
        }
        _port.setValue( eapsPort::setValueType::setTypeCurrent, value,
                        _ui->chb_adjustSetValue->isChecked() );
        _ui->lbl_setValueSet->setText( "set to " + QString::number( value )
                                       + UNIT_AMPERE );
    }
    else if( _ui->cob_setValue->currentText() == POWER_BY_VOLTAGE )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIWATT )
        {
            value /= 1000.0;
        }
        _port.setValue( eapsPort::setValueType::setTypePowerByVoltage, value,
                        _ui->chb_adjustSetValue->isChecked() );
        _ui->lbl_setValueSet->setText( "set to " + QString::number( value )
                                       + UNIT_WATT );
    }
    else if( _ui->cob_setValue->currentText() == POWER_BY_CURRENT )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIWATT )
        {
            value /= 1000.0;
        }
        _port.setValue( eapsPort::setValueType::setTypePowerByCurrent, value,
                        _ui->chb_adjustSetValue->isChecked() );
        _ui->lbl_setValueSet->setText( "set to " + QString::number( value )
                                       + UNIT_WATT );
    }
    else if( _ui->cob_setValue->currentText() == RESISTANCE_BY_VOLTAGE )
    {
        _port.setValue( eapsPort::setValueType::setTypeResistanceByVoltage,
                        value, _ui->chb_adjustSetValue->isChecked() );
        _ui->lbl_setValueSet->setText( "set to " + QString::number( value )
                                       + UNIT_OHM );
    }
    else if( _ui->cob_setValue->currentText() == RESISTANCE_BY_CURRENT )
    {
        _port.setValue( eapsPort::setValueType::setTypeResistanceByCurrent,
                        value, _ui->chb_adjustSetValue->isChecked() );
        _ui->lbl_setValueSet->setText( "set to " + QString::number( value )
                                       + UNIT_OHM );
    }

    _ui->lbl_setValueSet->setText( _ui->lbl_setValueSet->text() +
                                   (_ui->chb_adjustSetValue->isChecked() ?
                                        " (wa)" : " (na)") );
}

void eapsWindow::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.updateValues();
    }
}

void eapsWindow::voltageUpdate( double voltage )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": voltage update: "
              << voltage;
    _ui->txt_voltage->setText( QString::number( voltage ) + " " + UNIT_VOLT );

    if( _ui->chb_shareVoltage->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + VOLTAGE, voltage );
    }
}

void eapsWindow::currentUpdate( double current )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": current update: "
              << current;
    _ui->txt_current->setText( QString::number( current ) + " " + UNIT_AMPERE );

    if( _ui->chb_shareCurrent->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + CURRENT, current );
    }
}

void eapsWindow::powerUpdate( double power )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": power update: "
              << power;
    _ui->txt_power->setText( QString::number( power ) + " " + UNIT_WATT );

    if( _ui->chb_sharePower->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + POWER, power );
    }
}

void eapsWindow::resistanceUpdate( double resistance )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": resistance update: "
              << resistance;
    _ui->txt_resistance->setText( QString::number( resistance )
                                  + " " + UNIT_OHM );

    if( _ui->chb_shareResistance->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + RESISTANCE, resistance );
    }
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
    else if( text == RESISTANCE )
    {
        if( _ui->frame_resistance->isVisible() )
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
    else if( text == RESISTANCE )
    {
        _ui->frame_resistance->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_shareResistance->setChecked( false );
    }
    visibilitySelectionChanged();
    setPortEmits();
}

void eapsWindow::setValueSelectionChanged()
{
    _ui->cob_setValueUnit->clear();
    _ui->btn_setValue->setEnabled( _port.isRunning() );

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        _ui->cob_setValueUnit->addItem( UNIT_VOLT );
        _ui->cob_setValueUnit->addItem( UNIT_MILLIVOLT );
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        _ui->cob_setValueUnit->addItem( UNIT_AMPERE );
        _ui->cob_setValueUnit->addItem( UNIT_MILLIAMPERE );
    }
    else if( _ui->cob_setValue->currentText() == POWER_BY_VOLTAGE
             || _ui->cob_setValue->currentText() == POWER_BY_CURRENT )
    {
        _ui->cob_setValueUnit->addItem( UNIT_WATT );
        _ui->cob_setValueUnit->addItem( UNIT_MILLIWATT );

        _ui->btn_setValue->setEnabled( _ui->txt_voltage->text().size() > 3
                                       && _ui->txt_current->text().size() > 3
                                       && _port.isRunning() );
    }
    else if( _ui->cob_setValue->currentText() == RESISTANCE_BY_VOLTAGE
             || _ui->cob_setValue->currentText() == RESISTANCE_BY_CURRENT )
    {
        _ui->cob_setValueUnit->addItem( UNIT_OHM );

        _ui->btn_setValue->setEnabled( _ui->txt_voltage->text().size() > 3
                                       && _ui->txt_current->text().size() > 3
                                       && _port.isRunning() );
    }

    updateUnitRange();
}

void eapsWindow::updateUnitRange()
{
    _ui->dsp_setValue->setValue( 0.0 );

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_VOLT )
        {
            _ui->dsp_setValue->setMaximum( _port.maxVoltage() );
        }
        else if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIVOLT )
        {
            _ui->dsp_setValue->setMaximum( _port.maxVoltage()*1000 );
        }
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_AMPERE )
        {
            _ui->dsp_setValue->setMaximum( _port.maxCurrent() );
        }
        else if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIAMPERE )
        {
            _ui->dsp_setValue->setMaximum( _port.maxCurrent()*1000 );
        }
    }
    else if( _ui->cob_setValue->currentText() == POWER_BY_VOLTAGE
             || _ui->cob_setValue->currentText() == POWER_BY_CURRENT )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_WATT )
        {
            _ui->dsp_setValue->setMaximum(
                        _port.maxVoltage()*_port.maxCurrent() );
        }
        else if( _ui->cob_setValueUnit->currentText() == UNIT_MILLIWATT )
        {
            _ui->dsp_setValue->setMaximum(
                        _port.maxVoltage()*_port.maxCurrent()*1000 );
        }
    }
    else if( _ui->cob_setValue->currentText() == RESISTANCE_BY_VOLTAGE
             || _ui->cob_setValue->currentText() == RESISTANCE_BY_CURRENT )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_OHM )
        {
            _ui->dsp_setValue->setMaximum( 1000000 );
        }
    }
}

void eapsWindow::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );

    emit changeWindowState( this->windowTitle(), false );
}

void eapsWindow::resetInfo()
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

