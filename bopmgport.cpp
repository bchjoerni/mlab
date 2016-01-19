#include "bopmgport.h"

bopmgPort::bopmgPort( QObject *parent ) : labPort( parent ),
    _minVoltage( -100.0 ), _maxVoltage( 100.0 ),
    _minCurrent( -10.0 ), _maxCurrent( 10.0 ),
    _lastVoltage( 0.0 ), _lastCurrent( 0.0 ), _lastPower( 0.0 ),
    _setValueType( setValueType::setTypeNone ), _autoAdjust( false ),
    _emitVoltage( false ), _emitCurrent( false ), _emitPower( false )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
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
    _writingPauseMs     = 100;
    _bytesError         = 20;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void bopmgPort::getInitValues()
{    
    setCls();
    _expectedAnswer.push_back( CMD_OTHER );
    setRemoteControl( true );
    _expectedAnswer.push_back( CMD_OTHER );
    setOutput( true );
    _expectedAnswer.push_back( CMD_IDN );
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

    if( _emitVoltage || _emitPower )
    {
        getVoltage();
    }
    if( _emitCurrent || _emitPower )
    {
        getCurrent();
    }
    updateNumInTimeValues();

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

void bopmgPort::updateNumInTimeValues()
{
    _numInTimeValues = 0;
    /*
    if( _emitVoltage )
    {
        _numInTimeValues++;
    }
    if( _emitCurrent )
    {
        _numInTimeValues++;
    }
    if( _emitPower )
    {
        _numInTimeValues++;
    }
    */
}

void bopmgPort::setCls()
{
    sendBopmgCmd( "*CLS" );
}

void bopmgPort::setRemoteControl( bool on )
{
    sendBopmgCmd( QString( "SYST:REM " ) + (on ? "ON" : "OFF") );
}

void bopmgPort::setOutput( bool on )
{
    sendBopmgCmd( QString( "OUTP " ) + (on ? "ON" : "OFF") );
}

void bopmgPort::getIdn()
{
    sendBopmgCmd( CMD_IDN );
}

void bopmgPort::getMinVoltage()
{
    sendBopmgCmd( CMD_VOLTAGE_MIN );
}

void bopmgPort::getMaxVoltage()
{
    sendBopmgCmd( CMD_VOLTAGE_MAX );
}

void bopmgPort::getMinCurrent()
{
    sendBopmgCmd( CMD_CURRENT_MIN );
}

void bopmgPort::getMaxCurrent()
{
    sendBopmgCmd( CMD_CURRENT_MAX );
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
}

void bopmgPort::setVoltage( double voltage )
{
    _expectedAnswer.push_back( CMD_OTHER );
    sendBopmgCmd( "VOLT " + QString::number( voltage ) );
}

void bopmgPort::setCurrent( double current )
{
    _expectedAnswer.push_back( CMD_OTHER );
    sendBopmgCmd( "CURR " + QString::number( current ) );
}

void bopmgPort::getVoltage()
{
    sendBopmgCmd( CMD_VOLTAGE );
}

void bopmgPort::getCurrent()
{
    sendBopmgCmd( CMD_CURRENT );
}

void bopmgPort::sendBopmgCmd( QString cmd, bool inTime )
{
    QString msg = cmd + ";\r\n";
    if( cmd.contains( "?" ) )
    {
        _expectedAnswer.push_back( cmd );
    }
    sendMsg( msg.toStdString().c_str(), msg.size(), false ); // todo: in time not working properly
}

void bopmgPort::receivedMsg( QByteArray msg )
{
    if( _expectedAnswer.size() == 0 )
    {
        emit portError( "Unexpected answer!" );
        return;
    }

    msg.replace( 0x0A, "" ).replace( 0x0B, "" ).replace( 0x0D, "" )
            .replace( 0x11, "" ).replace( 0x13, "" );
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
