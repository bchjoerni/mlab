#include "eaps8000usbport.h"

eaps8000UsbPort::eaps8000UsbPort( QObject *parent ) : labPort( parent ),
    _inMsg( false ), _id( 0 ), _maxVoltage( 0.0 ), _maxCurrent( 0.0 ),
    _lastVoltage( 0.0 ), _lastCurrent( 0.0 ), _lastPower( 0.0 ),
    _lastResistance( 0.0 ),
    _setValueType( setValueType::setTypeNone ), _autoAdjust( false ),
    _emitVoltage( false ), _emitCurrent( false ), _emitPower( false ),
    _emitResistance( false ), _setPowerDirectly( false )
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

void eaps8000UsbPort::setPowerType( bool isAbleToSetPowerDirectly )
{
    _setPowerDirectly = isAbleToSetPowerDirectly;
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
        if( _lastVoltage > 0.0 && _lastResistance > 0.0 )
        {
            setVoltage( _lastVoltage*
                        std::pow( _setResistance/_lastResistance,
                                  adjustmentFactor ) );
        }
    }
    else if( _setValueType == setValueType::setTypeResistanceByCurrent )
    {
        if( _lastVoltage > 0.0 && _lastResistance > 0.0 )
        {
            setCurrent( _lastCurrent*
                        std::pow( _setResistance/_lastResistance,
                                  adjustmentFactor) );
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
        LOG(INFO) << "eaps 8000 usb set voltage: " << value
                  << " adjust: " << autoAdjust;
        _setVoltage = value;
        _lastVoltage = value;
        setVoltage( value );
    }
    else if( type == setValueType::setTypeCurrent )
    {
        LOG(INFO) << "eaps 8000 usb set current: " << value
                  << " adjust: " << autoAdjust;
        _setCurrent = value;
        _lastCurrent = value;
        setCurrent( value );
    }
    else if( type == setValueType::setTypePower )
    {
        LOG(INFO) << "eaps 8000 usb set power: " << value
                  << " adjust: " << autoAdjust;
        _setPower = value;
        _lastPower = value;
        setPower( value );
    }
    else if( type == setValueType::setTypePowerByVoltage
             || type == setValueType::setTypePowerByCurrent )
    {
        LOG(INFO) << "eaps 8000 usb set power: " << value
                  << " adjust: " << autoAdjust << " by voltage: "
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
        LOG(INFO) << "eaps 8000 usb set resistance: " << value << " adjust: "
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
    sendSetValue( 50, static_cast<int>( voltage/_maxVoltage*25600 ) );
}

void eaps8000UsbPort::setCurrent( double current )
{
    sendSetValue( 51, static_cast<int>( current/_maxCurrent*25600 ) );
}

void eaps8000UsbPort::setPower( double power )
{
    sendSetValue( 52, static_cast<int>( power/_maxPower*25600 ) );
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

    int checksum = msg[0] + msg[1] + msg[2];

    msg[3] = checksum/256;
    msg[4] = checksum%256;

    sendMsg( reinterpret_cast<char*>( msg ), 5, inTime );
}

void eaps8000UsbPort::sendSetValue( int object, int value )
{
    unsigned char msg[7];
    msg[0] = 0xF1;
    msg[1] = 0;
    msg[2] = object;
    msg[3] = value/256;
    msg[4] = value%256;

    int checksum = msg[0] + msg[1] + msg[2] + msg[3] + msg[4];

    msg[5] = checksum/256;
    msg[6] = checksum%256;

    sendMsg( reinterpret_cast<char*>( msg ), 7, false );
}

int eaps8000UsbPort::getAnswerLength( unsigned char startDelimiter )
{
    return startDelimiter%16 + 6;
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

        int answerLength = getAnswerLength( _bufferReceived.at( 0 ) );

        if( _bufferReceived.size() >= answerLength )
        {
            interpretMessage( _bufferReceived.left( answerLength ) );

            if( _bufferReceived.size() > answerLength )
            {
                _bufferReceived = _bufferReceived.mid( answerLength, -1 );
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
{/*
    qDebug() << "interpret message, " << msg.size() << " bytes:";
    for( int i = 0; i < msg.size(); i++ )
    {
        char c = msg.at( i );
        qDebug() << static_cast<int>( reinterpret_cast<unsigned char&>( c ) );
    }
*/
    char object = msg.at( 2 );
    switch( reinterpret_cast<unsigned char&>( object ) )
    {
        case 0: answerDeviceType( msg );
            break;
        case 1: answerDeviceSerial( msg );
            break;
        case 2: answerNominalVoltage( msg );
            break;
        case 3: answerNominalCurrent( msg );
            break;
        case 4: answerNominalPower( msg );
            break;
        case 6: answerArticleNumber( msg );
            break;
        case 7: answerUserText( msg );
            break;
        case 8: answerManufacturer( msg );
            break;
        case 9: answerSoftwareVersion( msg );
            break;
        case 10: answerInterfaceType( msg );
            break;
        case 11: answerInterfaceSerial( msg );
            break;
        case 12: answerInterfaceArticleNumber( msg );
            break;
        case 13: answerInterfaceFirmwareVersion( msg );
            break;
        case 19: answerDeviceClass( msg );
            break;
        case 22: unhandledAnswer( msg );
            break;
        case 23: unhandledAnswer( msg );
            break;
        case 24: unhandledAnswer( msg );
            break;
        case 25: unhandledAnswer( msg );
            break;
        case 26: unhandledAnswer( msg );
            break;
        case 27: unhandledAnswer( msg );
            break;
        case 28: unhandledAnswer( msg );
            break;
        case 29: unhandledAnswer( msg );
            break;
        case 30: unhandledAnswer( msg );
            break;
        case 31: unhandledAnswer( msg );
            break;
        case 37: unhandledAnswer( msg );
            break;
        case 38: unhandledAnswer( msg );
            break;
        case 50: answerSetVoltage( msg );
            break;
        case 51: answerSetCurrent( msg );
            break;
        case 52: answerSetPower( msg );
            break;
        case 54: unhandledAnswer( msg );
            break;
        case 70: answerDeviceState( msg );
            break;
        case 71: answerActualValues( msg );
            break;
        case 72: answerSetValues( msg );
            break;
        case 77: emit portError( "Communication error!" );
            break;
        case 190: unhandledAnswer( msg );
            break;
        case 191: unhandledAnswer( msg );
            break;
        case 192: unhandledAnswer( msg );
            break;
        case 193: unhandledAnswer( msg );
            break;
        case 194: unhandledAnswer( msg );
            break;
        case 255: answerError( msg );
            break;

        default: emit portError( "Unknown answer!" );
    }
}

void eaps8000UsbPort::unhandledAnswer( const QByteArray& msg )
{
    emit portError( "Unhandled Answer!" );
}

void eaps8000UsbPort::answerDeviceType( const QByteArray& msg )
{
    _idString = msg.mid( 3, msg.length() - 5 );
    _initValueCounter++;
    LOG(INFO) << "eaps 8000 usb port: answer device type: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
    emit initSuccessful( _idString );
}

void eaps8000UsbPort::answerDeviceSerial( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer device serial: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerNominalVoltage( const QByteArray& msg )
{
    byteConverter conv;
    _maxVoltage = conv.bytesToFloat( msg.mid( 3, 4 ).toStdString() );
    _initValueCounter++;
}

void eaps8000UsbPort::answerNominalCurrent( const QByteArray& msg )
{
    byteConverter conv;
    _maxCurrent = conv.bytesToFloat( msg.mid( 3, 4 ).toStdString() );
    _initValueCounter++;
}

void eaps8000UsbPort::answerNominalPower( const QByteArray& msg )
{
    byteConverter conv;
    _maxPower = conv.bytesToFloat( msg.mid( 3, 4 ).toStdString() );
    _initValueCounter++;
}

void eaps8000UsbPort::answerArticleNumber( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer article number: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerUserText( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer user text: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerManufacturer( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer manufacturer: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerSoftwareVersion( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer software version: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerInterfaceType( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer interface type: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerInterfaceSerial( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer interface serial: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerInterfaceArticleNumber( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer interface article number: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerInterfaceFirmwareVersion( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer interface firmware version: "
              << msg.mid( 3, msg.length() - 5 ).toStdString();
}

void eaps8000UsbPort::answerDeviceClass( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer device class: "
              << byteConverter::highLowCharsToInt( msg.at( 3 ), msg.at( 4 ) );
}

void eaps8000UsbPort::answerSetVoltage( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer set voltage: "
              << byteConverter::highLowCharsToInt( msg.at( 3 ), msg.at( 4 ) );
}

void eaps8000UsbPort::answerSetCurrent( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer set current: "
              << byteConverter::highLowCharsToInt( msg.at( 3 ), msg.at( 4 ) );
}

void eaps8000UsbPort::answerSetPower( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer set power: "
              << byteConverter::highLowCharsToInt( msg.at( 3 ), msg.at( 4 ) );
}

void eaps8000UsbPort::answerDeviceState( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer device state: "
              << byteConverter::highLowCharsToInt( msg.at( 3 ), msg.at( 4 ) );
}

void eaps8000UsbPort::answerActualValues( const QByteArray& msg )
{
    _inTimeValueCounter++;
    int voltageValue = byteConverter::highLowCharsToInt( msg.at( 3 ),
                                                         msg.at( 4 ) );
    int currentValue = byteConverter::highLowCharsToInt( msg.at( 5 ),
                                                         msg.at( 6 ) );
    int powerValue   = byteConverter::highLowCharsToInt( msg.at( 7 ),
                                                         msg.at( 8 ) );

    _lastVoltage    = voltageValue*_maxVoltage/25600.0;
    _lastCurrent    = currentValue*_maxCurrent/25600.0;
    _lastPower      = powerValue*_maxPower/25600.0;
    _lastResistance = _lastVoltage/_lastCurrent;

    if( _emitVoltage )
    {
        emit newVoltage( _lastVoltage );
    }
    if( _emitCurrent )
    {
        emit newCurrent( _lastCurrent );
    }
    if( _emitPower )
    {
        emit newPower( _lastPower );
    }
    if( _emitResistance )
    {
        emit newResistance( _lastResistance );
    }
}

void eaps8000UsbPort::answerSetValues( const QByteArray& msg )
{
    LOG(INFO) << "eaps 8000 usb port: answer set value: "
              << byteConverter::highLowCharsToInt( msg.at( 3 ), msg.at( 4 ) )
              << byteConverter::highLowCharsToInt( msg.at( 5 ), msg.at( 6 ) )
              << byteConverter::highLowCharsToInt( msg.at( 7 ), msg.at( 8 ) );
}

void eaps8000UsbPort::answerError( const QByteArray &msg )
{
    switch( msg.at( 3 ) )
    {
        case 0x01: emit portError( "RS232 error! (0x01)" ); // RS232: Parity error
            break;
        case 0x02: emit portError( "RS232 error! (0x02)" ); // RS232: Frame Error (Startbit or Stopbit incorrect)
            break;
        case 0x03: emit portError( "Protocol error! (0x03)" ); // Check sum incorrect
            break;
        case 0x04: emit portError( "Protocol error! (0x04)" ); // Start delimiter incorrect
            break;
        case 0x05: emit portError( "Protocol error! (0x05)" ); // CAN: max. nodes exceeded
            break;
        case 0x06: emit portError( "Protocol error! (0x06)" ); // Device node wrong / no gateway present
            break;
        case 0x07: emit portError( "Protocol error! (0x07)" ); // Object not defined
            break;
        case 0x08: emit portError( "Protocol error! (0x08)" ); // Object length incorrect
            break;
        case 0x09: emit portError( "Protocol error! (0x09)" ); // Read/Write permissions violated, no access
            break;
        case 0x0A: emit portError( "Protocol error! (0x0A)" ); // Time between two bytes too long / Number of bytes in message wrong
            break;
        case 0x0C: emit portError( "Protocol error! (0x0C)" ); // CAN: Split message aborted
            break;
        case 0x0F: emit portError( "Protocol error! (0x0F)" ); // Device is in "local" mode or analogue remote control
            break;
        case 0x10: emit portError( "Protocol error! (0x10)" ); // CAN driver chip: Stuffing error
            break;
        case 0x11: emit portError( "Protocol error! (0x11)" ); // CAN driver chip: CRC sum error
            break;
        case 0x12: emit portError( "Protocol error! (0x12)" ); // CAN driver chip: Form error
            break;
        case 0x13: emit portError( "Protocol error! (0x13)" ); // CAN: expected data length incorrect
            break;
        case 0x14: emit portError( "Protocol error! (0x14)" ); // CAN driver chip: Buffer full
            break;
        case 0x20: emit portError( "Protocol error! (0x20)" ); // Gateway: CAN Stuffing error
            break;
        case 0x21: emit portError( "Protocol error! (0x21)" ); // Gateway: CAN CRC check error
            break;
        case 0x22: emit portError( "Protocol error! (0x22)" ); // Gateway: CAN form error
            break;
        case 0x30: emit portError( "Protocol error! (0x30)" ); // Upper limit of object exceeded
            break;
        case 0x31: emit portError( "Protocol error! (0x31)" ); // Lower limit of object exceeded
            break;
        case 0x32: emit portError( "Protocol error! (0x32)" ); // Time definition not observed
            break;
        case 0x33: emit portError( "Protocol error! (0x33)" ); // Access to menu parameter only in standby
            break;
        case 0x36: emit portError( "Protocol error! (0x36)" ); // Access to function manager / function data denied
            break;
        case 0x38: emit portError( "Protocol error! (0x38)" ); // Access to object not possible
            break;

        default: emit portError( "Unknown answer error!" );
    }
}
