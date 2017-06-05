#include "eaps3000uicharwindow.h"
#include "ui_eaps3000uicharwindow.h"


eaps3000UICharWindow::eaps3000UICharWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::eaps3000UICharWindow ),
    _tickCounter( 0 ), _loopCounter( 0 ),
    _running( false ), _increasing( true ),
    _setUiValue( 0.0 ), _lastMeasuredValue( 0.0 )
{
    _ui->setupUi( this );

    connectPortFunctions();
    connectUiElements();

    addItems();
    refreshPortList();
    calculateRemainingTicks();

    _port.setEmitVoltage( true );
    _port.setEmitCurrent( true );
    _port.setEmitPower( true );
    _port.setEmitResistance( true );
}

eaps3000UICharWindow::~eaps3000UICharWindow()
{
    delete _ui;
}

void eaps3000UICharWindow::connectPortFunctions()
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

void eaps3000UICharWindow::connectUiElements()
{
    connect( _ui->btn_emergencyStop, SIGNAL( clicked() ), this,
             SLOT( emergencyStop() ) );
    connect( _ui->cob_setValue, SIGNAL( currentTextChanged( QString ) ), this,
             SLOT( setValueSelectionChanged() ) );
    connect( _ui->cob_unit, SIGNAL( currentTextChanged( QString ) ),
             this, SLOT( updateUnitRange() ) );

    connect( _ui->chb_calcValues, SIGNAL( stateChanged( int ) ), this,
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
    connect( _ui->btn_resetRefresh, SIGNAL( clicked() ), this,
             SLOT( resetRefresh() ) );
    connect( _ui->btn_clearInfo, SIGNAL( clicked() ), this,
             SLOT( clearInfo() ) );
}

void eaps3000UICharWindow::addItems()
{
    _ui->cob_setValue->addItem( VOLTAGE );
    _ui->cob_setValue->addItem( CURRENT );
}

void eaps3000UICharWindow::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );

            if( info.serialNumber().startsWith( "EAVI20R" ) )
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

void eaps3000UICharWindow::emergencyStop()
{
    _running = false;
    _tickCounter = 0;
    _ui->frame_setValues->setEnabled( true );
    _ui->btn_startStop->setText( START );
    calculateRemainingTicks();

    if( _port.isOpen() )
    {
        _port.setValue( eapsUta12Port::setValueType::setTypeVoltage,
                                0.0, false );
        _port.setValue( eapsUta12Port::setValueType::setTypeCurrent,
                                0.0, false );
    }

    _ui->lbl_info->setText( EMERGENCY_STOP );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );
    emit changeWindowState( this->windowTitle(), false );
}

void eaps3000UICharWindow::mLabSignal( const QString& cmd )
{
    QString cmdLower = cmd.toLower().trimmed();

    if( cmdLower == EMERGENCY_STOP.toLower() )
    {
        emergencyStop();
    }
    else if( cmdLower == STOP_SIGNAL.toLower() )
    {
        if( _ui->chb_setZeroAtStopSignal->isChecked() )
        {
            uiCharFinished();

            _ui->lbl_info->setText( STOP_INFO_TEXT );
            _ui->lbl_info->setStyleSheet( STYLE_ERROR );
        }
    }
    else if( cmdLower == "btn_startstop\tpress" )
    {
        startStop();
    }
    else if( cmdLower == "btn_start\tpress" )
    {
        if( _ui->btn_startStop->text() == START )
        {
            startStop();
        }
    }
    else if( cmdLower == "btn_stop\tpress" )
    {
        if( _ui->btn_startStop->text() == STOP )
        {
            startStop();
        }
    }
    else if( cmdLower == "btn_connectdisconnect\tpress" )
    {
        connectivityButtonPressed();
    }
    else if( cmdLower == "btn_connect\tpress" )
    {
        if( _ui->btn_connect->text() == CONNECT_PORT )
        {
            connectPort();
        }
    }
    else if( cmdLower == "btn_disconnect\tpress")
    {
        if( _ui->btn_connect->text() == DISCONNECT_PORT )
        {
            disconnectPort();
        }
    }
    else if( cmdLower == "chb_setzeroatstopsignal\ttrue" )
    {
        _ui->chb_setZeroAtStopSignal->setChecked( true );
    }
    else if( cmdLower == "chb_setzeroatstopsignal\tfalse" )
    {
        _ui->chb_setZeroAtStopSignal->setChecked( false );
    }
    else if( cmdLower == "chb_setzerowhenfinished\ttrue" )
    {
        _ui->chb_setZeroWhenFinished->setChecked( true );
    }
    else if( cmdLower == "chb_setzerowhenfinished\tfalse" )
    {
        _ui->chb_setZeroWhenFinished->setChecked( false );
    }
    else if( cmdLower == "chb_emitstopsignal\ttrue" )
    {
        _ui->chb_emitStopSignal->setChecked( true );
    }
    else if( cmdLower == "chb_emitstopsignal\tfalse" )
    {
        _ui->chb_emitStopSignal->setChecked( false );
    }
    else if( cmdLower == "btn_resetrefresh\tpress" )
    {
        resetRefresh();
    }
    else if( cmdLower == "btn_clearinfo\tpress" )
    {
        clearInfo();
    }
    else if( cmdLower.startsWith( "cob_setvalue\t" ) )
    {
        QString type = cmdLower.mid( cmdLower.indexOf( "\t" )+1 );
        int indexType = _ui->cob_setValue->findText( type );
        if( indexType == -1 )
        {
            return;
        }
        _ui->cob_setValue->setCurrentIndex( indexType );
    }
    else if( cmdLower.startsWith( "dsb_stepsize\t" ) )
    {
        QString value = cmdLower.mid( cmdLower.indexOf( "\t" )+1 );
        bool conversionSuccessful = false;
        double stepSize = value.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _ui->dsb_stepSize->setValue( stepSize );
        }
    }
    else if( cmdLower == "chb_calcvalues\ttrue" )
    {
        _ui->chb_calcValues->setChecked( true );
    }
    else if( cmdLower == "chb_calcvalues\tfalse" )
    {
        _ui->chb_calcValues->setChecked( false );
    }
    else if( cmdLower.startsWith( "dsb_fromvalue\t" ) )
    {
        QString value = cmdLower.mid( cmdLower.indexOf( "\t" )+1 );
        bool conversionSuccessful = false;
        double fromValue = value.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _ui->dsb_fromValue->setValue( fromValue );
        }
    }
    else if( cmdLower.startsWith( "dsb_tovalue\t" ) )
    {
        QString value = cmdLower.mid( cmdLower.indexOf( "\t" )+1 );
        bool conversionSuccessful = false;
        double toValue = value.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _ui->dsb_toValue->setValue( toValue );
        }
    }
    else if( cmdLower.startsWith( "cob_unit\t" ) )
    {
        QString unit = cmdLower.mid( cmdLower.indexOf( "\t" )+1 );
        int unitType = _ui->cob_unit->findText( unit );
        if( unitType == -1 )
        {
            return;
        }
        _ui->cob_unit->setCurrentIndex( unitType );
    }
    else if( cmdLower.startsWith( "spb_repeat\t" ) )
    {
        QString value = cmdLower.mid( cmdLower.indexOf( "\t" )+1 );
        bool conversionSuccessful = false;
        int repeat = value.toInt( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _ui->spb_repeat->setValue( repeat );
        }
    }
    else if( cmdLower == "chb_loop\ttrue" )
    {
        _ui->chb_loop->setChecked( true );
    }
    else if( cmdLower == "chb_loop\tfalse" )
    {
        _ui->chb_loop->setChecked( false );
    }
}

void eaps3000UICharWindow::connectivityButtonPressed()
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

void eaps3000UICharWindow::connectPort()
{
    _port.openPort( _ui->cob_ports->currentText() );
}

void eaps3000UICharWindow::disconnectPort()
{
    _port.closePort();

    _ui->btn_startStop->setEnabled( false );
    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );

    emit changeWindowState( this->windowTitle(), false );
}

void eaps3000UICharWindow::initFinished( const QString &idString )
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

void eaps3000UICharWindow::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.updateValues();

        if( _running )
        {
            setValues();
            _tickCounter++;
            _ui->lbl_ticksLeft->setText( QString::number(
                        _ui->lbl_ticksLeft->text().toInt() - 1 ) );
        }
    }
}

void eaps3000UICharWindow::setValues()
{
    if( endOfLoop() )
    {
        uiCharFinished();

        if( _ui->chb_emitStopSignal->isChecked() )
        {
            emit newSignal( SIGNAL_RECEIVER_ALL, STOP_SIGNAL );
        }
        return;
    }

    double setValue = (_ui->chb_calcValues->isChecked() ?
                           _setUiValue : _lastMeasuredValue);
    _setUiValue = setValue + (_increasing ? 1 : -1)*_ui->dsb_stepSize->value();

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        if( _ui->cob_unit->currentText() == UNIT_MILLIVOLT )
        {
            _setUiValue /= 1000.0;
        }
        _port.setValue( eapsUta12Port::setValueType::setTypeVoltage,
                        _setUiValue, false );
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        if( _ui->cob_unit->currentText() == UNIT_MILLIAMPERE )
        {
            _setUiValue /= 1000.0;
        }
        _port.setValue( eapsUta12Port::setValueType::setTypeCurrent,
                        _setUiValue, false );
    }
}

bool eaps3000UICharWindow::endOfLoop()
{
    if( inLoopInterval() )
    {
        if( _loopCounter < (_ui->chb_loop->isChecked() ? 1 : 0) +
                _ui->spb_repeat->value()*(_ui->chb_loop->isChecked() ? 2 : 1) )
        {
            _loopCounter++;
            if( _ui->chb_loop->isChecked() )
            {
                _increasing = !_increasing;
            }
            else
            {
                _setUiValue = _ui->dsb_fromValue->value() +
                        (_increasing ? -1 : 1)*_ui->dsb_stepSize->value();
                _lastMeasuredValue = _setUiValue;
            }
        }
        else
        {
            return true;
        }
    }

    return false;
}

bool eaps3000UICharWindow::inLoopInterval()
{
    if( _ui->dsb_fromValue->value() <= _ui->dsb_toValue->value() )
    {
        return (_increasing && (_setUiValue >= _ui->dsb_toValue->value()))
           || (!_increasing && (_setUiValue <= _ui->dsb_fromValue->value()));
    }
    else
    {
        return (_increasing && (_setUiValue >= _ui->dsb_fromValue->value()))
           || (!_increasing && (_setUiValue <= _ui->dsb_toValue->value()));
    }
}

void eaps3000UICharWindow::uiCharFinished()
{
    _running = false;
    _tickCounter = 0;
    _ui->frame_setValues->setEnabled( true );
    _ui->btn_startStop->setText( START );
    calculateRemainingTicks();

    if( _ui->chb_setZeroWhenFinished->isChecked() && _port.isOpen() )
    {
        if( _ui->cob_setValue->currentText() == VOLTAGE )
        {
            _port.setValue( eapsUta12Port::setValueType::setTypeVoltage,
                            0.0, false );
        }
        else if( _ui->cob_setValue->currentText() == CURRENT )
        {
            _port.setValue( eapsUta12Port::setValueType::setTypeCurrent,
                            0.0, false );
        }
    }

    emit changeWindowState( this->windowTitle(), false );
}

void eaps3000UICharWindow::startStop()
{
    calculateRemainingTicks();
    bool started = (_ui->btn_startStop->text() == START);

    _increasing = (_ui->dsb_fromValue->value() <= _ui->dsb_toValue->value());

    _ui->btn_startStop->setText( started ? STOP : START );
    _running = started;
    _tickCounter = 0;
    _loopCounter = 0;
    _ui->frame_setValues->setEnabled( !started );
    _setUiValue = _ui->dsb_fromValue->value() +
            (_increasing ? -1 : 1)*_ui->dsb_stepSize->value();
    _lastMeasuredValue = _setUiValue;

    emit changeWindowState( this->windowTitle(), started );
}

void eaps3000UICharWindow::voltageUpdate( double voltage )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": voltage = " << voltage << " V";
    LOG(INFO) << this->windowTitle().toStdString() << ": voltage update: "
              << voltage;

    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        _lastMeasuredValue = voltage;
    }
    _ui->txt_voltage->setText( QString::number( voltage ) + " V" );

    if( _ui->chb_shareVoltage->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + VOLTAGE, voltage );
    }
}

void eaps3000UICharWindow::currentUpdate( double current )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": current = " << current << " A";
    LOG(INFO) << this->windowTitle().toStdString() << ": current update: "
              << current;

    if( _ui->cob_setValue->currentText() == CURRENT )
    {
        _lastMeasuredValue = current;
    }
    _ui->txt_current->setText( QString::number( current ) + " A" );

    if( _ui->chb_shareCurrent->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + CURRENT, current );
    }
}

void eaps3000UICharWindow::powerUpdate( double power )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": power = " << power << " W";
    LOG(INFO) << this->windowTitle().toStdString() << ": power update: "
              << power;

    if( _ui->cob_setValue->currentText() == POWER )
    {
        _lastMeasuredValue = power;
    }
    _ui->txt_power->setText( QString::number( power ) + " W" );

    if( _ui->chb_sharePower->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + POWER, power );
    }
}

void eaps3000UICharWindow::resistanceUpdate( double resistance )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": resistance = " << resistance << " Ohm";
    LOG(INFO) << this->windowTitle().toStdString() << ": resistance update: "
              << resistance;

    if( _ui->cob_setValue->currentText() == RESISTANCE )
    {
        _lastMeasuredValue = resistance;
    }
    _ui->txt_resistance->setText( QString::number( resistance ) + " Ω" );

    if( _ui->chb_shareResistance->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + RESISTANCE, resistance );
    }
}

void eaps3000UICharWindow::setValueSelectionChanged()
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

void eaps3000UICharWindow::updateUnitRange()
{
    if( _ui->cob_setValue->currentText() == VOLTAGE )
    {
        if( _ui->cob_unit->currentText() == UNIT_VOLT )
        {
            _ui->dsb_fromValue->setMinimum( 0.0 );
            _ui->dsb_fromValue->setMaximum( _port.maxVoltage() );

            _ui->dsb_toValue->setMinimum( 0.0 );
            _ui->dsb_toValue->setMaximum( _port.maxVoltage() );
        }
        else if( _ui->cob_unit->currentText() == UNIT_MILLIVOLT )
        {
            _ui->dsb_fromValue->setMinimum( 0.0 );
            _ui->dsb_fromValue->setMaximum( _port.maxVoltage()*1000 );

            _ui->dsb_toValue->setMinimum( 0.0 );
            _ui->dsb_toValue->setMaximum( _port.maxVoltage()*1000 );
        }
    }
    else if( _ui->cob_setValue->currentText() == CURRENT )
    {
        if( _ui->cob_unit->currentText() == UNIT_AMPERE )
        {
            _ui->dsb_fromValue->setMinimum( 0.0 );
            _ui->dsb_fromValue->setMaximum( _port.maxCurrent() );

            _ui->dsb_toValue->setMinimum( 0.0 );
            _ui->dsb_toValue->setMaximum( _port.maxCurrent() );
        }
        else if( _ui->cob_unit->currentText() == UNIT_MILLIAMPERE )
        {
            _ui->dsb_fromValue->setMinimum( 0.0 );
            _ui->dsb_fromValue->setMaximum( _port.maxCurrent()*1000 );

            _ui->dsb_toValue->setMinimum( 0.0 );
            _ui->dsb_toValue->setMaximum( _port.maxCurrent()*1000 );
        }
    }
}

void eaps3000UICharWindow::fixStepSizeChanged()
{
    _ui->lbl_ticksApprox->setVisible( !_ui->chb_calcValues->isChecked() );
    if( !_ui->chb_calcValues->isChecked() )
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle( "Warning!" );
        msgBox.setText( "Warning: Using measured values might cause "
                        "problems!" );
        msgBox.exec();
    }
}

void eaps3000UICharWindow::calculateRemainingTicks()
{
    double steps = std::abs( _ui->dsb_toValue->value() -
                _ui->dsb_fromValue->value() )/_ui->dsb_stepSize->value();
    if( _ui->chb_loop->isChecked() )
    {
        steps *= 2;
    }
    steps *= (1 + _ui->spb_repeat->value());
    if( !_ui->chb_loop->isChecked() )
    {
        steps += _ui->spb_repeat->value();
    }
    _ui->lbl_ticksLeft->setText( QString::number(
                                     static_cast<int>( steps ) + 1 ) );
}

void eaps3000UICharWindow::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );

    emit newError( this->windowTitle() + ": " + error );
    emit changeWindowState( this->windowTitle(), false );
}

void eaps3000UICharWindow::resetRefresh()
{
    if( _ui->btn_connect->text() == DISCONNECT_PORT )
    {
        disconnectPort();
    }
    _port.reset();

    refreshPortList();
}

void eaps3000UICharWindow::clearInfo()
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
    }
}
