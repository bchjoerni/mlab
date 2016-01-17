#include "eapsport.h"

eapsPort::eapsPort( QObject *parent ) : labPort( parent ), _id( 0 ),
    _maxVoltageOut( 0 ), _maxCurrentOut( 0 ),
    _maxVoltageIn( 1 ), _maxCurrentIn( 1 ),
    _minVoltageValue( 0.0 ), _maxVoltageValue( 10.0 ),
    _minCurrentValue( 0.0 ), _maxCurrentValue( 10.0 ),
    _lastVoltage( 0.0 ), _lastCurrent( 0.0 ), _lastPower( 0.0 ),
    _setValueType( setValueType::setTypeNone ), _autoAdjust( false ),
    _emitVoltage( false ), _emitCurrent( false ), _emitPower( false )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
}

void eapsPort::setSerialValues()
{
    _port.setBaudRate( QSerialPort::Baud57600 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::NoParity );
}

void eapsPort::setLabPortVariables()
{
    _initTimeoutMs      = 1500;
    _initValueCounter   = 0;
    _numInitValues      = 4;
    _minBytesRead       = MESSAGE_LENGTH;
    _writingPauseMs     = 100;
    _bytesError         = 75;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void eapsPort::getInitValues()
{
    LOG(INFO) << "eaps: getIdn() ...";
    getIdn();
}

void eapsPort::updateValues()
{
    checkInTimeCount();

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

double eapsPort::maxVoltage() const
{
    return _maxVoltageValue;
}

double eapsPort::maxCurrent() const
{
    return _maxCurrentValue;
}

QString eapsPort::idString() const
{
    return _idString;
}

void eapsPort::setEmitVoltage( bool emitVoltage )
{
    _emitVoltage = emitVoltage;
}

void eapsPort::setEmitCurrent( bool emitCurrent )
{
    _emitCurrent = emitCurrent;
}

void eapsPort::setEmitPower( bool emitPower )
{
    _emitPower = emitPower;
}

void eapsPort::updateNumInTimeValues()
{
    _numInTimeValues = 0;
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
}

void eapsPort::adjustValues()
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

double eapsPort::calcAdjustedValue( double setValue, double lastValue )
{
    return (setValue - (lastValue - setValue)*adjustmentFactor);
}

void eapsPort::setValue( setValueType type, double value, bool autoAdjust )
{    
    _setValueType = type;
    if( type == setValueType::setTypeNone )
    {
        return;
    }
    _autoAdjust = autoAdjust;

    if( type == setValueType::setTypeVoltage )
    {
        LOG(INFO) << "eaps set voltage: " << value << " adjust: " << autoAdjust;
        _setVoltage = value;
        _lastVoltage = value;
        setVoltage( value );
    }
    else if( type == setValueType::setTypeCurrent )
    {
        LOG(INFO) << "eaps set current: " << value << " adjust: " << autoAdjust;
        _setCurrent = value;
        _lastCurrent = value;
        setCurrent( value );
    }
    else if( type == setValueType::setTypePowerByVoltage
             || type == setValueType::setTypePowerByCurrent )
    {
        LOG(INFO) << "eaps set power: " << value << " adjust: " << autoAdjust
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

void eapsPort::setVoltage( double voltage )
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x1F;
    int value = static_cast<int>( voltage*_maxVoltageOut/_maxVoltageValue );
    msg[4] = value/256;
    msg[5] = value%256;
    setCheckBytes( msg );
    sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::setCurrent( double current )
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x3F;
    int value = static_cast<int>( current*_maxCurrentOut/_maxCurrentValue );
    msg[4] = value/256;
    msg[5] = value%256;
    setCheckBytes( msg );
    sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::setRemoteControl( bool on )
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x5F;
    msg[4] = on ? 0x40 : 0;
    setCheckBytes( msg );
    sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::getIdString()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x8F;
    setCheckBytes( msg );
    sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::getVoltage()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x2F;
    setCheckBytes( msg );
    sendMsg( ((char*) &msg), MESSAGE_LENGTH, true );
}

void eapsPort::getCurrent()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x4F;
    setCheckBytes( msg );
    sendMsg( ((char*) &msg), MESSAGE_LENGTH, true );
}

void eapsPort::getIdn()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[3] = 0xFF;
    setCheckBytes( msg );
    sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::getMaxValues()
{
    unsigned char msg[MESSAGE_LENGTH];
    setToPrototype( msg );
    msg[2] = _id;
    msg[3] = 0x48;
    setCheckBytes( msg );
    sendMsg( ((char*) &msg), MESSAGE_LENGTH, false );
}

void eapsPort::setToPrototype( unsigned char *msg )
{
    for( int i = 0; i < MESSAGE_LENGTH; i++ )
    {
        msg[i] = MESSAGE_PROTOTYPE[i];
    }
}

void eapsPort::setCheckBytes( unsigned char* msg )
{
    int checksum = calcCheckBytes( msg );
    msg[MESSAGE_LENGTH - 2] = checksum/256;
    msg[MESSAGE_LENGTH - 1] = checksum%256;
}

int eapsPort::calcCheckBytes( unsigned char *msg )
{
    int checksum = 0;
    for( int i = 0; i < MESSAGE_LENGTH - 2; i++ )
    {
        checksum += msg[i];
    }
    return (256*256 - 1 - checksum);
}

bool eapsPort::checkMsgBytes( unsigned char *msg )
{
    return calcCheckBytes( msg )/256 == msg[MESSAGE_LENGTH - 2]
            && calcCheckBytes( msg )%256 == msg[MESSAGE_LENGTH - 1];
}

bool eapsPort::checkAnswerFormat( QByteArray msg, unsigned char* msgValues )
{
    if( msg.size() != MESSAGE_LENGTH )
    {
        emit portError( "Invalid answer length!" );
        return false;
    }
    for( int i = 0; i < MESSAGE_LENGTH; i++ )
    {
        msgValues[i] = (unsigned char) msg.at( i );
    }
    if( !checkMsgBytes( msgValues ) )
    {
        LOG(INFO) << "eaps check bytes wrong!";
        return false;
    }
    return true;
}

void eapsPort::receivedMsg( QByteArray msg )
{
    unsigned char msgValues[MESSAGE_LENGTH];
    if( !checkAnswerFormat( msg, msgValues ) )
    {
        return;
    }

    switch( msgValues[3] )
    {
        case 0x1F:
        {
            answerSetVoltage();
            break;
        }
        case 0x2F:
        {
            answerGetVoltage( msgValues );
            break;
        }
        case 0x3F:
        {
            answerSetCurrent();
            break;
        }
        case 0x4F:
        {
            answerGetCurrent( msgValues );
            break;
        }
        case 0x5F:
        {
            answerSetStatus();
            break;
        }
        case 0x6F:
        {
            answerGetStatus( msgValues );
            break;
        }
        case 0x7F:
        {
            answerSetIdString();
            break;
        }
        case 0x8F:
        {
            answerGetIdString( msg );
            break;
        }
        case 0x9F:
        {
            answerEcho( msgValues );
            break;
        }
        case 0xAF:
        {
            answerError( msgValues );
            break;
        }
        case 0xEF:
        {
            answerSetIdn();
            break;
        }
        case 0xFF:
        {            
            answerGetIdn( msgValues );
            break;
        }
        case 0x48:
        {
            answerGetMaxValues( msgValues );
            break;
        }
        case 0xC8:
        {
            answerSetUserText();
            break;
        }
        case 0xD8:
        {
            answerGetUserText( msg );
            break;
        }
    default: emit portError( "Unknown answer!" );
            break;
    }
}

void eapsPort::answerSetVoltage()
{
    LOG(INFO) << "eaps answer: set voltage";
}

void eapsPort::answerGetVoltage( unsigned char* msgValues )
{
    int value = msgValues[4]*256 + msgValues[5];
    _lastVoltage = _maxVoltageValue*value/_maxVoltageIn;
    _inTimeValueCounter++;
    emit newVoltage( _lastVoltage );
}

void eapsPort::answerSetCurrent()
{
    LOG(INFO) << "eaps answer: set current";
}

void eapsPort::answerGetCurrent( unsigned char* msgValues )
{
    int value = msgValues[4]*256 + msgValues[5];
    _lastCurrent = _maxCurrentValue*value/_maxCurrentIn;
    _inTimeValueCounter++;
    emit newCurrent( _lastCurrent );
    if( _emitPower )
    {
        _inTimeValueCounter++;
        emit newPower( _lastVoltage*_lastCurrent );
    }
}

void eapsPort::answerSetStatus()
{
    _initValueCounter++;
    emit initSuccessful( _idString );
}

void eapsPort::answerGetStatus( unsigned char* msgValues )
{
    LOG(INFO) << "eaps answer: get status";
}

void eapsPort::answerSetIdString()
{
    LOG(INFO) << "eaps answer: set ID string";
}

void eapsPort::answerGetIdString( QByteArray msg )
{
    _idString = msg.mid( 4, MESSAGE_LENGTH-6 );
    int pos = _idString.indexOf( "30" );
    if( pos > 0 )
    {
        bool conversionSuccessful = false;
        int vmax = _idString.mid( pos+2, 2 ).toInt(
                    &conversionSuccessful );
        if( conversionSuccessful )
        {
            _maxVoltageValue = vmax;
        }
        int imax = _idString.mid( pos+5, 2 ).toInt(
                    &conversionSuccessful );
        if( conversionSuccessful )
        {
            _maxCurrentValue = imax;
        }
    }
    LOG(INFO) << "eaps answer: get ID string: " << _idString.toStdString();
    _idString = "ID string: " + _idString;
    _initValueCounter++;
    setRemoteControl( true );
}

void eapsPort::answerEcho( unsigned char* msgValues )
{
    LOG(INFO) << "eaps answer: echo";
}

void eapsPort::answerError( unsigned char* msgValues )
{
    emit portError( "Communication error! ("
                    + QString::number( msgValues[5] ) + ")" );
}

void eapsPort::answerSetIdn()
{
    LOG(INFO) << "eaps answer: set IDN";
}

void eapsPort::answerGetIdn( unsigned char* msgValues )
{
    _id = msgValues[4];
    _initValueCounter++;
    LOG(INFO) << "eaps answer: get IDN: " << _id;
    getMaxValues();
}

void eapsPort::answerGetMaxValues( unsigned char* msgValues )
{
    _maxVoltageOut = msgValues[4]*256 + msgValues[5];
    _maxCurrentOut = msgValues[6]*256 + msgValues[7];
    _maxVoltageIn  = msgValues[8]*256 + msgValues[9];
    _maxCurrentIn = msgValues[10]*256 + msgValues[11];
    _initValueCounter++;
    LOG(INFO) << "eaps answer: get max values:"
              << " maxVoltageOut: " << _maxVoltageOut
              << " maxCurrentOut: " << _maxCurrentOut
              << " maxVoltageIn: "  << _maxVoltageIn
              << " maxCurrentIn: "  << _maxCurrentIn;
    getIdString();
}

void eapsPort::answerSetUserText()
{
    LOG(INFO) << "eaps answer: set user text";
}

void eapsPort::answerGetUserText( QByteArray msg )
{
    LOG(INFO) << "eaps answer: get user text";
}
