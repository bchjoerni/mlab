#include "bopmguicharwindow.h"
#include "ui_bopmguicharwindow.h"

bopmgUICharWindow::bopmgUICharWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::bopmgUICharWindow ), _tickCounter( 0 ), _running( false ),
    _valueForTickCalc( 0.0 )
{
    _ui->setupUi( this );

    connectPortFunctions();
    connectUiElements();

    addItems();
    refreshPortList();
    calculateRemainingTicks();

    emitVoltageChanged();
    emitCurrentChanged();
    emitPowerChanged();
}

bopmgUICharWindow::~bopmgUICharWindow()
{
    delete _ui;
}

void bopmgUICharWindow::connectPortFunctions()
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
}

void bopmgUICharWindow::connectUiElements()
{
    connect( _ui->cob_setValue, SIGNAL( currentTextChanged( QString ) ), this,
             SLOT( setValueSelectionChanged() ) );
    connect( _ui->cob_unit, SIGNAL( currentTextChanged( QString ) ),
             this, SLOT( updateUnitRange() ) );

    connect( _ui->chb_shareVoltage, SIGNAL( stateChanged( int ) ), this,
             SLOT( emitVoltageChanged() ) );
    connect( _ui->chb_shareCurrent, SIGNAL( stateChanged( int ) ), this,
             SLOT( emitCurrentChanged() ) );
    connect( _ui->chb_sharePower, SIGNAL( stateChanged( int ) ), this,
             SLOT( emitPowerChanged() ) );
    connect( _ui->chb_fixedSteps, SIGNAL( stateChanged( int ) ), this,
             SLOT( fixStepSizeChanged() ) );

    connect( _ui->dsb_fromValue, SIGNAL( valueChanged( double ) ), this,
             SLOT( calculateRemainingTicks() ) );
    connect( _ui->dsb_toValue, SIGNAL( valueChanged( double ) ), this,
             SLOT( calculateRemainingTicks() ) );
    connect( _ui->dsb_stepSize, SIGNAL( valueChanged( double ) ), this,
             SLOT( calculateRemainingTicks() ) );
    connect( _ui->spb_repeat, SIGNAL( valueChanged( int ) ), this,
             SLOT( calculateRemainingTicks() ) );
    connect( _ui->chb_loop, SIGNAL( stateChanged( int ) ), this,
             SLOT( calculateRemainingTicks() ) );

    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_startStop, SIGNAL( clicked() ), this,
             SLOT( startStop() ) );
    connect( _ui->btn_resetInfo, SIGNAL( clicked() ), this,
             SLOT( resetInfo() ) );
}

void bopmgUICharWindow::addItems()
{
    _ui->cob_setValue->addItem( VOLTAGE );
    _ui->cob_setValue->addItem( CURRENT );
}

void bopmgUICharWindow::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );
        }
    }

    _ui->btn_connect->setEnabled( _ui->cob_ports->count() > 0 );
    _ui->cob_ports->setEnabled( _ui->cob_ports->count() > 0 );

    if( _ui->cob_ports->count() < 1 )
    {
        _ui->cob_ports->addItem( NO_PORTS_AVAILABLE );
    }
}

void bopmgUICharWindow::connectivityButtonPressed()
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

void bopmgUICharWindow::connectPort()
{
    _port.openPort( _ui->cob_ports->currentText() );
}

void bopmgUICharWindow::disconnectPort()
{
    _port.closePort();

    _ui->btn_startStop->setEnabled( false );
    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );
}

void bopmgUICharWindow::initFinished( const QString &idString )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": init finished, id: "
              << idString.toStdString();
    _ui->lbl_info->setText( idString );

    _ui->btn_startStop->setEnabled( true );
    _ui->lbl_status->setText( CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
    _ui->btn_connect->setText( DISCONNECT_PORT );

    updateUnitRange();
}

void bopmgUICharWindow::doUpdate()
{
    if( _port.isRunning() )
    {
        if( _running )
        {
            setValues();
            _tickCounter++;
        }

        _port.updateValues();
    }
}

void bopmgUICharWindow::setValues()
{
    double value = _ui->dsb_fromValue->value()
            + _tickCounter*_ui->dsb_stepSize->value();

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        if( _ui->cob_unit->currentText() == UNIT_MILLIVOLT )
        {
            value /= 1000.0;
        }
        _port.setValue( bopmgPort::setValueType::setTypeVoltage, value, false );
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        if( _ui->cob_unit->currentText() == UNIT_MILLIAMPERE )
        {
            value /= 1000.0;
        }
        _port.setValue( bopmgPort::setValueType::setTypeCurrent, value, false );
    }
}

void bopmgUICharWindow::startStop()
{
    if( _ui->dsb_fromValue->value() > _ui->dsb_toValue->value() )
    {
        double temp = _ui->dsb_fromValue->value();
        _ui->dsb_fromValue->setValue( _ui->dsb_toValue->value() );
        _ui->dsb_toValue->setValue( temp );
    }
    bool started = (_ui->btn_startStop->text() == START);

    _running = started;
    _ui->frame_setValues->setEnabled( !started );
}

void bopmgUICharWindow::voltageUpdate( double voltage )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": voltage update: "
              << voltage;
    _ui->txt_voltage->setText( QString::number( voltage ) + " V" );
    emit newValue( this->windowTitle() + ": " + VOLTAGE, voltage );
}

void bopmgUICharWindow::currentUpdate( double current )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": current update: "
              << current;
    _ui->txt_current->setText( QString::number( current ) + " A" );
    emit newValue( this->windowTitle() + ": " + CURRENT, current );
}

void bopmgUICharWindow::powerUpdate( double power )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": power update: "
              << power;
    _ui->txt_power->setText( QString::number( power ) + " W" );
    emit newValue( this->windowTitle() + ": " + POWER, power );
}

void bopmgUICharWindow::emitVoltageChanged()
{
    _port.setEmitVoltage( _ui->chb_shareVoltage->isChecked() );
}

void bopmgUICharWindow::emitCurrentChanged()
{
    _port.setEmitCurrent( _ui->chb_shareCurrent->isChecked() );
}

void bopmgUICharWindow::emitPowerChanged()
{
    _port.setEmitPower( _ui->chb_sharePower->isChecked() );
}

void bopmgUICharWindow::setValueSelectionChanged()
{
    _ui->cob_unit->clear();

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        _ui->cob_unit->addItem( UNIT_VOLT );
        _ui->cob_unit->addItem( UNIT_MILLIVOLT );
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        _ui->cob_unit->addItem( UNIT_AMPERE );
        _ui->cob_unit->addItem( UNIT_MILLIAMPERE );
    }

    updateUnitRange();
}

void bopmgUICharWindow::updateUnitRange()
{
    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        if( _ui->cob_unit->currentText() == UNIT_VOLT )
        {
            _ui->dsb_fromValue->setMinimum( _port.minVoltage() );
            _ui->dsb_fromValue->setMaximum( _port.maxVoltage() );

            _ui->dsb_toValue->setMinimum( _port.minVoltage() );
            _ui->dsb_toValue->setMaximum( _port.maxVoltage() );
        }
        else if( _ui->cob_unit->currentText() == UNIT_MILLIVOLT )
        {
            _ui->dsb_fromValue->setMinimum( _port.minVoltage()*1000 );
            _ui->dsb_fromValue->setMaximum( _port.maxVoltage()*1000 );

            _ui->dsb_toValue->setMinimum( _port.minVoltage()*1000 );
            _ui->dsb_toValue->setMaximum( _port.maxVoltage()*1000 );
        }
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        if( _ui->cob_unit->currentText() == UNIT_AMPERE )
        {
            _ui->dsb_fromValue->setMinimum( _port.minCurrent() );
            _ui->dsb_fromValue->setMaximum( _port.maxCurrent() );

            _ui->dsb_toValue->setMinimum( _port.minCurrent() );
            _ui->dsb_toValue->setMaximum( _port.maxCurrent() );
        }
        else if( _ui->cob_unit->currentText() == UNIT_MILLIAMPERE )
        {
            _ui->dsb_fromValue->setMinimum( _port.minCurrent()*1000 );
            _ui->dsb_fromValue->setMaximum( _port.maxCurrent()*1000 );

            _ui->dsb_toValue->setMinimum( _port.minCurrent()*1000 );
            _ui->dsb_toValue->setMaximum( _port.maxCurrent()*1000 );
        }
    }
}

void bopmgUICharWindow::fixStepSizeChanged()
{
    _ui->lbl_ticksApprox->setVisible( !_ui->chb_fixedSteps->isChecked() );
}

void bopmgUICharWindow::calculateRemainingTicks()
{
    if( _running )
    {
        if( _ui->cob_setValue->currentText() == VOLTAGE )
        {
            _valueForTickCalc = _ui->txt_voltage->text().toDouble();
        }
        else if( _ui->cob_setValue->currentText() == CURRENT )
        {
            _valueForTickCalc = _ui->txt_current->text().toDouble();
        }
    }
    else
    {
        _valueForTickCalc = _ui->dsb_fromValue->value();
    }

    double steps = std::abs( _ui->dsb_toValue->value() - _valueForTickCalc )
            /_ui->dsb_stepSize->value();
    if( _ui->chb_loop->isChecked() )
    {
        steps *= 2;
    }
    steps *= (1 + _ui->spb_repeat->value());
    _ui->lbl_ticksLeft->setText( QString::number(
                                     static_cast<int>( steps ) + 1 ) );
}

void bopmgUICharWindow::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );
}

void bopmgUICharWindow::resetInfo()
{
    _port.clearErrors();

    if( _port.isRunning() )
    {
        _ui->lbl_info->setText( _port.idString() );
        _ui->lbl_info->setStyleSheet( "" );
    }
    else
    {
        _ui->lbl_info->setText( "-" );
        _ui->lbl_info->setStyleSheet( "" );
    }
    refreshPortList();
}
