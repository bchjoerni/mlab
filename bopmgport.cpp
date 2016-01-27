#include "bopmgport.h"

bopmgPort::bopmgPort( QObject *parent ) : labPort( parent ),
    _setValueType( setValueType::setTypeNone ),
    _sendCounter( 0 ), _answerPending( -1 ),
    _minVoltage( -100.0 ), _maxVoltage( 100.0 ),
    _minCurrent( -10.0 ), _maxCurrent( 10.0 ),
    _lastVoltage( 0.0 ), _lastCurrent( 0.0 ), _lastPower( 0.0 ),
    _lastResistance( 0.0 ), _autoAdjust( false ),
    _emitVoltage( false ), _emitCurrent( false ), _emitPower( false ),
    _emitResistance( false )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
    connect( &_checkForAnswerTimer, SIGNAL( timeout() ), this,
             SLOT( nextMsg() ) );
}

void bopmgPort::setSerialValues()
{
    _port.setBaudRate( QSerialPort::Baud19200 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::NoParity );
}

void bopmgPort::setLabPortVariables()
{
    _initTimeoutMs      = 1500;
    _initValueCounter   = 0;
    _numInitValues      = 1;
    _minBytesRead       = -10;
    _writingPauseMs     = 50;
    _bytesError         = 20;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void bopmgPort::getInitValues()
{    
    setCls();
    setRemoteControl( true );
    setOutput( true );
    getIdn();
}

void bopmgPort::updateValues()
{
    checkInTimeCount();
    if( _expectedAnswer.size() != 0 )
    {
        _expectedAnswer.clear();
        emit portError( "Answer missing!" );
    }

    if( _emitVoltage || _emitPower || _emitResistance )
    {
        getVoltage();
    }
    if( _emitCurrent || _emitPower || _emitResistance )
    {
        getCurrent();
    }

    if( _setValueType != setValueType::setTypeNone
            && _autoAdjust )
    {
        adjustValues();
    }
}

void bopmgPort::setEmitVoltage( bool emitVoltage )
{
    _emitVoltage = emitVoltage;
}

void bopmgPort::setEmitCurrent( bool emitCurrent )
{
    _emitCurrent = emitCurrent;
}

void bopmgPort::setEmitPower( bool emitPower )
{
    _emitPower = emitPower;
}

void bopmgPort::setEmitResistance( bool emitResistance )
{
    _emitResistance = emitResistance;
}

void bopmgPort::setCls()
{
    sendBopmgCmd( "*CLS", false );
}

void bopmgPort::setRemoteControl( bool on )
{
    sendBopmgCmd( QString( "SYST:REM " ) + (on ? "ON" : "OFF"), false );
}

void bopmgPort::setOutput( bool on )
{
    sendBopmgCmd( QString( "OUTP " ) + (on ? "ON" : "OFF"), false );
}

void bopmgPort::getIdn()
{
    sendBopmgCmd( CMD_IDN, true );
}

void bopmgPort::getMinVoltage()
{
    sendBopmgCmd( CMD_VOLTAGE_MIN, true );
}

void bopmgPort::getMaxVoltage()
{
    sendBopmgCmd( CMD_VOLTAGE_MAX, true );
}

void bopmgPort::getMinCurrent()
{
    sendBopmgCmd( CMD_CURRENT_MIN, true );
}

void bopmgPort::getMaxCurrent()
{
    sendBopmgCmd( CMD_CURRENT_MAX, true );
}

double bopmgPort::minVoltage() const
{
    return _minVoltage;
}

double bopmgPort::maxVoltage() const
{
    return _maxVoltage;
}

double bopmgPort::minCurrent() const
{
    return _minCurrent;
}

double bopmgPort::maxCurrent() const
{
    return _maxCurrent;
}

QString bopmgPort::idString() const
{
    return _idString;
}

void bopmgPort::adjustValues()
{
    if( _setValueType == setValueType::setTypePowerByVoltage )
    {
        setVoltage( calcAdjustedValue( _setVoltage, _lastVoltage ) );
    }
    else if( _setValueType == setValueType::setTypeCurrent )
    {
        setCurrent( calcAdjustedValue( _setCurrent, _lastCurrent ) );
    }
    else if( _setValueType == setValueType::setTypePowerByVoltage )
    {
        if( _lastVoltage > 0.0 && _lastCurrent > 0.0 )
        {
            setVoltage( std::sqrt( calcAdjustedValue( _setPower, _lastPower )*
                                   _lastVoltage/_lastCurrent ) );
        }
    }
    else if( _setValueType == setValueType::setTypePowerByCurrent )
    {
        if( _lastVoltage > 0.0 && _lastCurrent > 0.0 )
        {
            setCurrent( std::sqrt( calcAdjustedValue( _setPower, _lastPower )/
                                   (_lastVoltage/_lastCurrent) ) );
        }
    }
    else if( _setValueType == setValueType::setTypeResistanceByVoltage )
    {
        if( _lastVoltage > 0.0 && _lastCurrent > 0.0 )
        {
            setVoltage( calcAdjustedValue( _setResistance,
                        _lastVoltage/_lastCurrent )*_lastCurrent );
        }
    }
    else if( _setValueType == setValueType::setTypeResistanceByCurrent )
    {
        if( _lastVoltage > 0.0 && _lastCurrent > 0.0 )
        {
            setCurrent( 1.0/(calcAdjustedValue( _setResistance,
                        _lastVoltage/_lastCurrent )/_lastVoltage) );
        }
    }
}

double bopmgPort::calcAdjustedValue( double setValue, double lastValue )
{
    return (setValue - (lastValue - setValue)*adjustmentFactor);
}

void bopmgPort::setValue( setValueType type, double value, bool autoAdjust )
{
    _setValueType = type;
    if( type == setValueType::setTypeNone )
    {
        return;
    }
    _autoAdjust = autoAdjust;

    if( type == setValueType::setTypeVoltage )
    {
        LOG(INFO) << "bopmg set voltage: " << value
                  << " adjust: " << autoAdjust;
        _setVoltage = value;
        _lastVoltage = value;
        setVoltage( value );
    }
    else if( type == setValueType::setTypeCurrent )
    {
        LOG(INFO) << "bopmg set current: " << value
                  << " adjust: " << autoAdjust;
        _setCurrent = value;
        _lastCurrent = value;
        setCurrent( value );
    }
    else if( type == setValueType::setTypePowerByVoltage
             || type == setValueType::setTypePowerByCurrent )
    {
        LOG(INFO) << "Bopmg set power: " << value << " adjust: " << autoAdjust
                  << " by voltage: "
                  << (type == setValueType::setTypePowerByVoltage);
        _setPower = value;
        _lastPower = value;
        if( _lastVoltage > 0.0 && _lastCurrent > 0.0 )
        {
            double resistance = _lastVoltage/_lastCurrent;
            if( type == setValueType::setTypePowerByVoltage )
            {
                setVoltage( std::sqrt( value*resistance ) );
            }
            else if( type == setValueType::setTypePowerByCurrent )
            {
                setCurrent( std::sqrt( value/resistance ) );
            }
        }
        else
        {
            emit portError( "Measure before setting power!" );
        }
    }
    else if( type == setValueType::setTypeResistanceByVoltage
             || type == setValueType::setTypeResistanceByCurrent )
    {
        LOG(INFO) << "bopmg set resistance: " << value << " adjust: "
                  << autoAdjust << " by voltage: "
                  << (type == setValueType::setTypeResistanceByVoltage);
        _setResistance = value;
        _lastResistance = value;
        if( _lastVoltage > 0.0 && _lastCurrent > 0.0 )
        {
            if( type == setValueType::setTypeResistanceByVoltage )
            {
                setVoltage( _lastCurrent*value );
            }
            else if( type == setValueType::setTypeResistanceByCurrent )
            {
                setCurrent( _lastVoltage/value );
            }
        }
        else
        {
            emit portError( "Measure before setting power!" );
        }
    }
}

void bopmgPort::setVoltage( double voltage )
{
    sendBopmgCmd( "VOLT " + QString::number( voltage ), false );
}

void bopmgPort::setCurrent( double current )
{
    sendBopmgCmd( "CURR " + QString::number( current ), false );
}

void bopmgPort::getVoltage()
{
    sendBopmgCmd( CMD_VOLTAGE, true );
}

void bopmgPort::getCurrent()
{
    sendBopmgCmd( CMD_CURRENT, true );
}

void bopmgPort::sendBopmgCmd( QString cmd, bool answer )
{
    if( answer )
    {
        if( cmd != CMD_IDN
                && cmd != CMD_VOLTAGE
                && cmd != CMD_CURRENT
                && cmd != CMD_VOLTAGE_MIN
                && cmd != CMD_VOLTAGE_MAX
                && cmd != CMD_CURRENT_MIN
                && cmd != CMD_CURRENT_MAX )
        {
            _expectedAnswer.push_back( CMD_OTHER );
        }
        else
        {
            _expectedAnswer.push_back( cmd );
        }
    }
    _msgToSend.push_back( (answer ? "y" : "n") + cmd + "\r\n" );    

    if( !_checkForAnswerTimer.isActive() )
    {
        _answerPending = -1;
        nextMsg();
    }
}

void bopmgPort::nextMsg()
{
    if( _answerPending == 1 )
    {
        noAnswerReceived();
    }
    else
    {
        if( _msgToSend.size() == 0 )
        {
            _checkForAnswerTimer.stop();
        }
        sendNextMessage();
    }
}

void bopmgPort::noAnswerReceived()
{
    if( _sendCounter < TRIES_SEND_MSG )
    {
        _sendCounter++;
        sendMsg( _msgToSend[0].toStdString().c_str(), _msgToSend[0].size(),
                 false );
        _checkForAnswerTimer.start( _writingPauseMs*2 );
    }
    else
    {
        emit portError( "No answer after several requests!" );
        _sendCounter = 0;
        _msgToSend.erase( _msgToSend.begin() );
        _expectedAnswer.erase( _expectedAnswer.begin() );
        _answerPending = -1;
        if( _msgToSend.size() > 0 )
        {
            _checkForAnswerTimer.start( _writingPauseMs*2 );
        }
    }
}

void bopmgPort::sendNextMessage()
{
    _sendCounter = 0;
    if( _answerPending != -1 && _msgToSend.size() > 0 )
    {
        _msgToSend.erase( _msgToSend.begin() );
    }

    if( _msgToSend.size() > 0 )
    {
        _answerPending = (_msgToSend[0].at( 0 ) == 'n') ? 0 : 1;
        _msgToSend[0] = _msgToSend[0].mid( 1, -1 );
        sendMsg( _msgToSend[0].toStdString().c_str(), _msgToSend[0].size(),
                 false );
        _checkForAnswerTimer.start( _writingPauseMs*2 );
    }
}

void bopmgPort::receivedMsg( QByteArray msg )
{
    msg.replace( 0x0A, "" ).replace( 0x0B, "" ).replace( 0x0D, "" )
            .replace( 0x11, "" ).replace( 0x13, "" );
    if( msg.isEmpty() )
    {
        return;
    }

    if( _expectedAnswer.size() == 0 )
    {
        emit portError( "Unexpected answer!" );
        return;
    }

    _answerPending = 0;   
    bool conversionSuccessful = false;
    if( _expectedAnswer[0] == CMD_VOLTAGE )
    {
        double voltage = msg.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _lastVoltage = voltage;
            emit newVoltage( _lastVoltage );
        }
    }
    else if( _expectedAnswer[0] == CMD_CURRENT )
    {
        double current = msg.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _lastCurrent = current;
            emit newCurrent( _lastCurrent );
        }
        if( _emitPower )
        {
            _lastPower = _lastVoltage*_lastCurrent;
            emit newPower( _lastPower );
        }
        if( _emitResistance )
        {
            _lastResistance = _lastVoltage/_lastCurrent;
            emit newResistance( _lastResistance );
        }
    }
    else if( _expectedAnswer[0] == CMD_IDN )
    {
        _idString = msg.left( 20 );
        _initValueCounter++;
        emit initSuccessful( _idString );
    }
    else if( _expectedAnswer[0] == CMD_VOLTAGE_MIN )
    {
        double value = msg.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _minVoltage = value;
            _initValueCounter++;
        }
    }
    else if( _expectedAnswer[0] == CMD_VOLTAGE_MAX )
    {
        double value = msg.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _maxVoltage = value;
            _initValueCounter++;
        }
    }
    else if( _expectedAnswer[0] == CMD_CURRENT_MIN )
    {
        double value = msg.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _minCurrent = value;
            _initValueCounter++;
        }
    }
    else if( _expectedAnswer[0] == CMD_CURRENT_MAX )
    {
        double value = msg.toDouble( &conversionSuccessful );
        if( conversionSuccessful )
        {
            _maxCurrent = value;
            _initValueCounter++;
        }
    }
    else if( _expectedAnswer[0] == CMD_OTHER )
    {
    }
    else
    {
        emit portError( "Undefined answer!" );
    }
    _expectedAnswer.erase( _expectedAnswer.begin() );
}
