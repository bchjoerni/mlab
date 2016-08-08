#include "eaps8000usbport.h"

eaps8000UsbPort::eaps8000UsbPort( QObject *parent ) : labPort( parent ),
    _inMsg( false ), _id( 0 ), _maxVoltage( 0.0 ), _maxCurrent( 0.0 ),
    _lastVoltage( 0.0 ), _lastCurrent( 0.0 ), _lastPower( 0.0 ),
    _lastResistance( 0.0 ),
    _setValueType( setValueType::setTypeNone ), _autoAdjust( false ),
    _emitVoltage( false ), _emitCurrent( false ), _emitPower( false ),
    _emitResistance( false )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
}

void eaps8000UsbPort::setSerialValues()
{
    _port.setBaudRate( QSerialPort::Baud57600 );
    _port.setDataBits( QSerialPort::Data8 );
    _port.setStopBits( QSerialPort::OneStop );
    _port.setParity(   QSerialPort::OddParity );
}

void eaps8000UsbPort::setLabPortVariables()
{
    _initTimeoutMs      = 2000;
    _initValueCounter   = 0;
    _numInitValues      = 4;
    _minBytesRead       = 0;
    _writingPauseMs     = 150;
    _bytesError         = 75;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 0;
}

void eaps8000UsbPort::getInitValues()
{
    setRemoteControl( true );
    getMaxValues();
    getIdString();
}

void eaps8000UsbPort::updateValues()
{
    checkInTimeCount();

    if( _emitVoltage || _emitCurrent || _emitPower || _emitResistance )
    {
        getCurrentValues();
    }
    updateNumInTimeValues();

    if( _setValueType != setValueType::setTypeNone
            && _autoAdjust )
    {
        adjustValues();
    }
}

double eaps8000UsbPort::maxVoltage() const
{
    return _maxVoltage;
}

double eaps8000UsbPort::maxCurrent() const
{
    return _maxCurrent;
}

double eaps8000UsbPort::maxPower() const
{
    return _maxPower;
}

QString eaps8000UsbPort::idString() const
{
    return _idString;
}

void eaps8000UsbPort::setEmitVoltage( bool emitVoltage )
{
    _emitVoltage = emitVoltage;
}

void eaps8000UsbPort::setEmitCurrent( bool emitCurrent )
{
    _emitCurrent = emitCurrent;
}

void eaps8000UsbPort::setEmitPower( bool emitPower )
{
    _emitPower = emitPower;
}

void eaps8000UsbPort::setEmitResistance( bool emitResistance )
{
    _emitResistance = emitResistance;
}

void eaps8000UsbPort::updateNumInTimeValues()
{
    _numInTimeValues = 0;

    if( _emitVoltage || _emitCurrent || _emitPower || _emitResistance )
    {
        _numInTimeValues++;
    }
}

void eaps8000UsbPort::adjustValues()
{
    if( _setValueType == setValueType::setTypeVoltage )
    {
        setVoltage( calcAdjustedValue( _setVoltage, _lastVoltage ) );
    }
    else if( _setValueType == setValueType::setTypeCurrent )
    {
        setCurrent( calcAdjustedValue( _setCurrent, _lastCurrent ) );
    }
    else if( _setValueType == setValueType::setTypePower )
    {
        setPower( calcAdjustedValue( _setPower, _lastPower ) );
    }
    else if( _setValueType == setValueType::setTypeResistanceByVoltage )
    {
        if( _lastVoltage > 0.0 && _lastResistance > 0.0 )
        {
            setVoltage( _lastVoltage*
                        std::sqrt( _setResistance/_lastResistance ) );
        }
    }
    else if( _setValueType == setValueType::setTypeResistanceByCurrent )
    {
        if( _lastVoltage > 0.0 && _lastResistance > 0.0 )
        {
            setCurrent( _lastCurrent*
                        std::sqrt( _setResistance/_lastResistance ) );
        }
    }
}

double eaps8000UsbPort::calcAdjustedValue( double setValue, double lastValue )
{
    return (setValue - (lastValue - setValue)*adjustmentFactor);
}

void eaps8000UsbPort::setValue( setValueType type, double value,
                                bool autoAdjust )
{
    _setValueType = type;
    if( type == setValueType::setTypeNone )
    {
        return;
    }
    _autoAdjust = autoAdjust;

    if( type == setValueType::setTypeVoltage )
    {
        LOG(INFO) << "eaps 8000 set voltage: " << value
                  << " adjust: " << autoAdjust;
        _setVoltage = value;
        _lastVoltage = value;
        setVoltage( value );
    }
    else if( type == setValueType::setTypeCurrent )
    {
        LOG(INFO) << "eaps 8000 set current: " << value
                  << " adjust: " << autoAdjust;
        _setCurrent = value;
        _lastCurrent = value;
        setCurrent( value );
    }
    else if( type == setValueType::setTypePower )
    {
        LOG(INFO) << "eaps 8000 set power: " << value
                  << " adjust: " << autoAdjust;
        _setPower = value;
        _lastPower = value;
        setPower( value );
    }
    else if( type == setValueType::setTypeResistanceByVoltage
             || type == setValueType::setTypeResistanceByCurrent )
    {
        LOG(INFO) << "eaps 8000 set resistance: " << value << " adjust: "
                  << autoAdjust << " by voltage: "
                  << (type == setValueType::setTypeResistanceByVoltage);
        _setResistance = value;
        if( _lastVoltage > 0.0 && _lastCurrent > 0.0 && _lastResistance > 0.0 )
        {
            if( type == setValueType::setTypeResistanceByVoltage )
            {
                setVoltage( _lastVoltage*
                            std::sqrt( _setResistance/_lastResistance ) );
            }
            else if( type == setValueType::setTypeResistanceByCurrent )
            {
                setCurrent( _lastCurrent*
                            std::sqrt( _setResistance/_lastResistance ) );
            }
        }
        else
        {
            emit portError( "Measure before setting power!" );
        }
    }
}

void eaps8000UsbPort::setVoltage( double voltage )
{
    sendSetValue( 50, static_cast<int>( voltage/_maxVoltage*256 ) );
}

void eaps8000UsbPort::setCurrent( double current )
{
    sendSetValue( 51, static_cast<int>( current/_maxCurrent*256 ) );
}

void eaps8000UsbPort::setPower( double power )
{
    sendSetValue( 52, static_cast<int>( power/_maxPower*256 ) );
}

void eaps8000UsbPort::setRemoteControl( bool on )
{
    unsigned char msg[7];
    msg[0] = 0xF1;
    msg[1] = 0x00;
    msg[2] = 0x36;
    msg[3] = 0x10;
    msg[4] = on ? 0x10 : 0x00;

    int checksum = msg[0] + msg[1] + msg[2] + msg[3] + msg[4];

    msg[5] = checksum/256;
    msg[6] = checksum%256;

    sendMsg( reinterpret_cast<char*>( msg ), 7, false );
}

void eaps8000UsbPort::getIdString()
{
    sendGetValue( 0, false );
}

void eaps8000UsbPort::getCurrentValues()
{
    sendGetValue( 71, true );
}

void eaps8000UsbPort::getMaxValues()
{
    getMaxVoltage();
    getMaxCurrent();
    getMaxPower();
}

void eaps8000UsbPort::getMaxVoltage()
{
    sendGetValue( 2, false );
}

void eaps8000UsbPort::getMaxCurrent()
{
    sendGetValue( 3, false );
}

void eaps8000UsbPort::getMaxPower()
{
    sendGetValue( 4, false );

}

void eaps8000UsbPort::sendGetValue( int object, bool inTime )
{
    unsigned char msg[5];

    unsigned char startDelimiter;
    switch( getMessageLength( object ) )
    {
        case 2: startDelimiter = 0b00000001;
            break;
        case 4: startDelimiter = 0b00000011;
            break;
        case 6: startDelimiter = 0b00000101;
            break;
        case 16: startDelimiter = 0b00001111;
            break;
        default: return; // TODO error handling
    }
    msg[0] = startDelimiter + 0b01110000; // 0b01010000 for single cast
    msg[1] = 0;
    msg[2] = object;

    int checksum = msg[0] + msg[1] + msg[2] + msg[3] + msg[4];

    msg[5] = checksum/256;
    msg[6] = checksum%256;

    sendMsg( reinterpret_cast<char*>( msg ), 5, inTime );
}

void eaps8000UsbPort::sendSetValue( int object, int value )
{
    unsigned char msg[7];
    msg[0] = 0xF1;
    msg[1] = 0;
    msg[2] = object;
    msg[3] = value%256;
    msg[4] = value/256;

    int checksum = msg[0] + msg[1] + msg[2] + msg[3] + msg[4];

    msg[5] = checksum/256;
    msg[6] = checksum%256;

    sendMsg( reinterpret_cast<char*>( msg ), 7, false );
}

int eaps8000UsbPort::getAnswerLength( unsigned char startDelimiter )
{
    return startDelimiter%16 + 5;
}

char eaps8000UsbPort::getMessageType( unsigned char objectByte )
{
    char charChar   = 'c';
    char floatChar  = 'f';
    char intChar    = 'i';
    char stringChar = 's';

    switch( objectByte )
    {
        case 0: return stringChar;
        case 1: return stringChar;
        case 2: return floatChar;
        case 3: return floatChar;
        case 4: return floatChar;
        case 6: return stringChar;
        case 7: return stringChar;
        case 8: return stringChar;
        case 9: return stringChar;
        case 10: return stringChar;
        case 11: return stringChar;
        case 12: return stringChar;
        case 13: return stringChar;
        case 19: return intChar;
        case 22: return intChar;
        case 23: return intChar;
        case 24: return intChar;
        case 25: return intChar;
        case 26: return intChar;
        case 27: return intChar;
        case 28: return intChar;
        case 29: return intChar;
        case 30: return intChar;
        case 31: return intChar;
        case 37: return intChar;
        case 38: return intChar;
        case 50: return intChar;
        case 51: return intChar;
        case 52: return intChar;
        case 54: return charChar;
        case 70: return intChar;
        case 71: return intChar;
        case 72: return intChar;
        case 77: return intChar;
        case 190: return intChar;
        case 191: return intChar;
        case 192: return intChar;
        case 193: return stringChar;
        case 194: return intChar;

        default: emit portError( "Protocol error!" );
            return 'e';
    }
}

int eaps8000UsbPort::getMessageLength( unsigned char objectByte )
{
    switch( objectByte )
    {
        case 0: return 16;
        case 1: return 16;
        case 2: return 4;
        case 3: return 4;
        case 4: return 4;
        case 6: return 16;
        case 7: return 16;
        case 8: return 16;
        case 9: return 16;
        case 10: return 16;
        case 11: return 16;
        case 12: return 16;
        case 13: return 16;
        case 19: return 2;
        case 22: return 6;
        case 23: return 4;
        case 24: return 6;
        case 25: return 4;
        case 26: return 6;
        case 27: return 4;
        case 28: return 6;
        case 29: return 4;
        case 30: return 6;
        case 31: return 4;
        case 37: return 2;
        case 38: return 2;
        case 50: return 2;
        case 51: return 2;
        case 52: return 2;
        case 54: return 2;
        case 70: return 2;
        case 71: return 6;
        case 72: return 6;
        case 77: return 6;
        case 190: return 4;
        case 191: return 4;
        case 192: return 4;
        case 193: return 16;
        case 194: return 2;

        default: emit portError( "Protocol error!" );
            return 0;
    }
}

void eaps8000UsbPort::receivedMsg( QByteArray msg )
{
    if( msg.isEmpty() )
    {
        return;
    }

    _bufferReceived.append( msg );

    if( _bufferReceived.size() >= 3 )
    {
        /*
        if( getMessageType( _bufferReceived.at( 2 ) ) == 's' )
        {

        }
        else
        {*/

        if( _bufferReceived.size() >=
                getAnswerLength( _bufferReceived.at( 0 ) ) )
        {
            interpretMessage( _bufferReceived.left(
                    getAnswerLength( _bufferReceived.at( 0 ) ) ) );

            if( _bufferReceived.size() >
                    getAnswerLength( _bufferReceived.at( 0 ) ) )
            {
                _bufferReceived.mid( getAnswerLength( _bufferReceived.at( 0 ) ),
                                     -1 );
            }
            else
            {
                _bufferReceived.clear();
            }
        }

        //}
    }
}

void eaps8000UsbPort::interpretMessage( QByteArray msg )
{
    char object = msg.at( 2 );
    switch( reinterpret_cast<unsigned char&>( object ) )
    {
        case 0: answerDeviceType();
            break;
        case 1: answerDeviceSerial();
            break;
        case 2: answerNominalVoltage();
            break;
        case 3: answerNominalCurrent();
            break;
        case 4: answerNominalPower();
            break;
        case 6: answerArticleNumber();
            break;
        case 7: answerUserText();
            break;
        case 8: answerManufacturer();
            break;
        case 9: answerSoftwareVersion();
            break;
        case 10: answerInterfaceType();
            break;
        case 11: answerInterfaceSerial();
            break;
        case 12: answerInterfaceArticleNumber();
            break;
        case 13: answerInterfaceFirmwareVersion();
            break;
        case 19: answerDeviceClass();
            break;
        case 22: unhandledAnswer();
        case 23: unhandledAnswer();
        case 24: unhandledAnswer();
        case 25: unhandledAnswer();
        case 26: unhandledAnswer();
        case 27: unhandledAnswer();
        case 28: unhandledAnswer();
        case 29: unhandledAnswer();
        case 30: unhandledAnswer();
        case 31: unhandledAnswer();
        case 37: unhandledAnswer();
        case 38: unhandledAnswer();
        case 50: answerSetVoltage();
        case 51: answerSetCurrent();
        case 52: answerSetPower();
        case 54: unhandledAnswer();
        case 70: answerDeviceState();
        case 71: answerActualValues();
        case 72: answerSetValues();
        case 77: emit portError( "Communication error!" );
        case 190: unhandledAnswer();
        case 191: unhandledAnswer();
        case 192: unhandledAnswer();
        case 193: unhandledAnswer();
        case 194: unhandledAnswer();

        default: emit portError( "Protocol Error!" );
    }
}

void eaps8000UsbPort::unhandledAnswer()
{
    emit portError( "Unhandled Answer!" );
}

void eaps8000UsbPort::answerDeviceType()
{
    _idString = _bufferReceived.mid( 3, _bufferReceived.length() - 5 );
    _initValueCounter++;
    LOG(INFO) << "eaps 8000 usb port: answer device type: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerDeviceSerial()
{
    LOG(INFO) << "eaps 8000 usb port: answer device serial: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerNominalVoltage()
{
    byteConverter conv;
    _maxVoltage = conv.bytesToFloat( _bufferReceived.mid( 4, 4 )
                                     .toStdString() );
    _initValueCounter++;
}

void eaps8000UsbPort::answerNominalCurrent()
{
    byteConverter conv;
    _maxCurrent = conv.bytesToFloat( _bufferReceived.mid( 4, 4 )
                                     .toStdString() );
    _initValueCounter++;

}

void eaps8000UsbPort::answerNominalPower()
{
    byteConverter conv;
    _maxPower = conv.bytesToFloat( _bufferReceived.mid( 4, 4 )
                                     .toStdString() );
    _initValueCounter++;
}

void eaps8000UsbPort::answerArticleNumber()
{
    LOG(INFO) << "eaps 8000 usb port: answer article number: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerUserText()
{
    LOG(INFO) << "eaps 8000 usb port: answer user text: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerManufacturer()
{
    LOG(INFO) << "eaps 8000 usb port: answer manufacturer: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerSoftwareVersion()
{
    LOG(INFO) << "eaps 8000 usb port: answer software version: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerInterfaceType()
{
    LOG(INFO) << "eaps 8000 usb port: answer interface type: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerInterfaceSerial()
{
    LOG(INFO) << "eaps 8000 usb port: answer interface serial: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerInterfaceArticleNumber()
{
    LOG(INFO) << "eaps 8000 usb port: answer interface article number: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerInterfaceFirmwareVersion()
{
    LOG(INFO) << "eaps 8000 usb port: answer interface firmware version: "
              << _bufferReceived.mid( 3, _bufferReceived.length() - 5 )
                 .toStdString();
}

void eaps8000UsbPort::answerDeviceClass()
{
    LOG(INFO) << "eaps 8000 usb port: answer device class: "
              << _bufferReceived.at( 3 )*256 + _bufferReceived.at( 4 );
}

void eaps8000UsbPort::answerSetVoltage()
{
    LOG(INFO) << "eaps 8000 usb port: answer set voltage: "
              << _bufferReceived.at( 3 )*256 + _bufferReceived.at( 4 );
}

void eaps8000UsbPort::answerSetCurrent()
{
    LOG(INFO) << "eaps 8000 usb port: answer set current: "
              << _bufferReceived.at( 3 )*256 + _bufferReceived.at( 4 );
}

void eaps8000UsbPort::answerSetPower()
{
    LOG(INFO) << "eaps 8000 usb port: answer set power: "
              << _bufferReceived.at( 3 )*256 + _bufferReceived.at( 4 );
}

void eaps8000UsbPort::answerDeviceState()
{
    LOG(INFO) << "eaps 8000 usb port: answer device state: "
              << _bufferReceived.at( 3 ) << " " << _bufferReceived.at( 4 );
}

void eaps8000UsbPort::answerActualValues()
{
    _inTimeValueCounter++;
    int voltageValue = _bufferReceived.at( 3 )*256 + _bufferReceived.at( 4 );
    int currentValue = _bufferReceived.at( 5 )*256 + _bufferReceived.at( 6 );
    int powerValue   = _bufferReceived.at( 7 )*256 + _bufferReceived.at( 8 );

    if( _emitVoltage )
    {
        emit newVoltage( voltageValue*_maxVoltage/256.0 );
    }
    if( _emitCurrent )
    {
        emit newCurrent( currentValue*_maxCurrent/256.0 );
    }
    if( _emitPower )
    {
        emit newPower( powerValue*_maxPower/256.0 );
    }
    if( _emitResistance )
    {
        emit newResistance( voltageValue*_maxVoltage/
                            (currentValue*_maxCurrent) );
    }
}

void eaps8000UsbPort::answerSetValues()
{
    LOG(INFO) << "eaps 8000 usb port: answer set value: "
              << _bufferReceived.at( 3 ) << " " << _bufferReceived.at( 4 )
              << _bufferReceived.at( 5 ) << " " << _bufferReceived.at( 6 )
              << _bufferReceived.at( 7 ) << " " << _bufferReceived.at( 8 );
}
