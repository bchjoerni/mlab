#include "elflowwindow.h"
#include "ui_elflowwindow.h"

elFlowWindow::elFlowWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::elFlowWindow )
{
    _ui->setupUi( this );
    _ui->frame_pressure->setVisible( false );
    _ui->frame_temperature->setVisible( false );

    connectPortFunctions();
    connectUiElements();

    addItems();
    refreshPortList();
    visibilitySelectionChanged();
}

elFlowWindow::~elFlowWindow()
{
    delete _ui;
}

void elFlowWindow::connectPortFunctions()
{
    connect( &_port, SIGNAL( initSuccessful( QString ) ), this,
             SLOT( initFinished( QString ) ) );
    connect( &_port, SIGNAL( portError( QString ) ), this,
             SLOT( portError( QString ) ) );
    connect( &_port, SIGNAL( newFlow( double ) ), this,
             SLOT( flowUpdate( double ) ) );
    connect( &_port, SIGNAL( newTemperature( double ) ), this,
             SLOT( temperatureUpdate( double ) ) );
    connect( &_port, SIGNAL( newPressure( double ) ), this,
             SLOT( pressureUpdate( double ) ) );
}

void elFlowWindow::connectUiElements()
{
    connect( _ui->btn_emergencyStop, SIGNAL( clicked() ), this,
             SLOT( emergencyStop() ) );
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

void elFlowWindow::addItems()
{
    _ui->cob_setValue->addItem( FLOW );

    _ui->cob_measuredValues->addItem( FLOW );
    _ui->cob_measuredValues->addItem( PRESSURE );
    _ui->cob_measuredValues->addItem( TEMPERATURE );
}

void elFlowWindow::setPortEmits()
{
    _port.setEmitFlow(        _ui->frame_flow->isVisible() );
    _port.setEmitPressure(    _ui->frame_pressure->isVisible() );
    _port.setEmitTemperature( _ui->frame_temperature->isVisible() );
}

void elFlowWindow::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );

            if( info.hasProductIdentifier() )
            {
                if( info.productIdentifier() == 2303 )
                {
                     _ui->cob_ports->setCurrentText( info.portName() );
                }
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

void elFlowWindow::mLabSignal( char signal )
{
    if( signal == SHUTDOWN_SIGNAL )
    {
        emergencyStop();
    }
    else if( signal == STOP_SIGNAL && _port.isOpen() )
    {
        _port.setValue( elFlowPort::setValueType::setTypeFlow, 0, false );

        _ui->lbl_info->setText( STOP_RECEIVED );
        _ui->lbl_info->setStyleSheet( STYLE_ERROR );
        emit changeWindowState( this->windowTitle(), false );
    }
}

void elFlowWindow::emergencyStop()
{
    if( _port.isOpen() )
    {
        _port.setValue( elFlowPort::setValueType::setTypeFlow, 0, false );

        _ui->lbl_info->setText( EMERGENCY_STOP );
        _ui->lbl_info->setStyleSheet( STYLE_ERROR );
        emit changeWindowState( this->windowTitle(), false );
    }
}

void elFlowWindow::connectivityButtonPressed()
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

void elFlowWindow::connectPort()
{
    _port.setId( _ui->spb_id->value() );
    setPortEmits();
    _port.openPort( _ui->cob_ports->currentText() );
}

void elFlowWindow::disconnectPort()
{
    _port.closePort();

    _ui->btn_setValue->setEnabled( false );
    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );
    _ui->lbl_setValueSet->setText( "-" );

    emit changeWindowState( this->windowTitle(), false );
}

void elFlowWindow::initFinished( const QString &idString )
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

void elFlowWindow::setValue()
{
    double value = _ui->dsp_setValue->value();

    if( _ui->cob_setValue->currentText() == FLOW )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_SCCM )
        {
            value *= 100.0/_port.maxCapacity();
        }
        _port.setValue( elFlowPort::setValueType::setTypeFlow, value,
                        _ui->chb_adjustSetValue->isChecked() );
        _ui->lbl_setValueSet->setText( "set to " + QString::number(
                                           value*_port.maxCapacity()/100.0 )
                                       + UNIT_SCCM + " / " + QString::number(
                                           value ) + UNIT_PERCENT );
        _ui->frame_flow->setVisible( true );
    }

    _ui->lbl_setValueSet->setText( _ui->lbl_setValueSet->text() +
                                   (_ui->chb_adjustSetValue->isChecked() ?
                                        " (wa)" : " (na)") );
    setPortEmits();
}

void elFlowWindow::doUpdate()
{
    if( _port.isRunning() )
    {
        _port.updateValues();
    }
}

void elFlowWindow::flowUpdate( double flow )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
                    << ": flow = " << flow*_port.maxCapacity()/100.0
                    << " sccm / "  << flow << " %";
    LOG(INFO) << this->windowTitle().toStdString() << ": flow update: "
              << flow;
    _ui->txt_flow->setText( QString::number( flow*_port.maxCapacity()/100.0 )
                            + " " + UNIT_SCCM + " / " +
                            QString::number( flow ) + " " + UNIT_PERCENT );

    if( _ui->chb_shareFlow->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + FLOW,
                       flow*_port.maxCapacity()/100.0 );
    }
}

void elFlowWindow::pressureUpdate( double pressure )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
               << ": pressure = " << pressure << " mbar";
    LOG(INFO) << this->windowTitle().toStdString() << ": pressure update: "
              << pressure;
    _ui->txt_pressure->setText( QString::number( pressure ) + " "
                                + UNIT_MBAR );

    if( _ui->chb_sharePressure->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + PRESSURE, pressure );
    }
}

void elFlowWindow::temperatureUpdate( double temperature )
{
    CLOG(INFO, "v") << this->windowTitle().toStdString()
               << ": temperature = " << temperature << " C";
    LOG(INFO) << this->windowTitle().toStdString() << ": temperature update: "
              << temperature;
    _ui->txt_temperature->setText( QString::number( temperature ) + " "
                                   + UNIT_DEGC );

    if( _ui->chb_shareTemperature->isChecked() )
    {
        emit newValue( this->windowTitle() + ": " + TEMPERATURE, temperature );
    }
}

void elFlowWindow::visibilitySelectionChanged()
{
    QString text = _ui->cob_measuredValues->currentText();

    if( text == FLOW )
    {
        if( _ui->frame_flow->isVisible() )
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
    else if( text == TEMPERATURE )
    {
        if( _ui->frame_temperature->isVisible() )
        {
            _ui->btn_measuredValuesVisibility->setText( HIDE );
        }
        else
        {
            _ui->btn_measuredValuesVisibility->setText( SHOW );
        }
    }
}

void elFlowWindow::changeVisibility()
{
    QString text = _ui->cob_measuredValues->currentText();

    if( text == FLOW )
    {
        _ui->frame_flow->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_shareFlow->setChecked( false );
    }
    else if( text == PRESSURE )
    {
        _ui->frame_pressure->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_sharePressure->setChecked( false );
    }
    else if( text == TEMPERATURE )
    {
        _ui->frame_temperature->setVisible(
                    _ui->btn_measuredValuesVisibility->text() == SHOW );
        _ui->chb_shareTemperature->setChecked( false );
    }
    visibilitySelectionChanged();
    setPortEmits();
}

void elFlowWindow::setValueSelectionChanged()
{
    _ui->cob_setValueUnit->clear();
    _ui->btn_setValue->setEnabled( _port.isRunning() );

    if( _ui->cob_setValue->currentText() == FLOW )
    {
        _ui->cob_setValueUnit->addItem( UNIT_PERCENT );
        _ui->cob_setValueUnit->addItem( UNIT_SCCM );
    }

    updateUnitRange();
}

void elFlowWindow::updateUnitRange()
{
    _ui->dsp_setValue->setValue( 0.0 );

    if( _ui->cob_setValue->currentText() == FLOW )
    {
        if( _ui->cob_setValueUnit->currentText() == UNIT_PERCENT )
        {
            _ui->dsp_setValue->setMaximum( 100 );
        }
        else if( _ui->cob_setValueUnit->currentText() == UNIT_SCCM )
        {
            _ui->dsp_setValue->setMaximum( _port.maxCapacity() );
        }
    }
}

void elFlowWindow::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );

    emit changeWindowState( this->windowTitle(), false );
}

void elFlowWindow::resetInfo()
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

