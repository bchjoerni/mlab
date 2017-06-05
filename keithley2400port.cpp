#include "keithley2400port.h"


keithley2400Port::keithley2400Port( QObject *parent ) : labPort( parent ),
    _sendCounter( 0 ), _answerPending( -1 ),
    _lastResistance( 0.0 ), _emitResistance( false )
{
    setSerialValues();
    setLabPortVariables();

    connect( this, SIGNAL( dataReceived( QByteArray ) ), this,
                           SLOT( receivedMsg( QByteArray ) ) );
    connect( &_checkForAnswerTimer, SIGNAL( timeout() ), this,
             SLOT( nextMsg() ) );
}

void keithley2400Port::setSerialValues()
{
    _port->setBaudRate( QSerialPort::Baud19200 );
    _port->setDataBits( QSerialPort::Data8 );
    _port->setStopBits( QSerialPort::OneStop );
    _port->setParity(   QSerialPort::NoParity );
}

void keithley2400Port::setLabPortVariables()
{
    _initTimeoutMs      = 1500;
    _initValueCounter   = 0;
    _numInitValues      = 1;
    _minBytesRead       = -10;
    _writingPauseMs     = 200;
    _bytesError         = 20;
    _inTimeValueCounter = 0;
    _numInTimeValues    = 1;
}

void keithley2400Port::getInitValues()
{
    setCls();
//    setRemoteControl( true );
//    setOutput( true );
//    setConfigResistance();
    getIdn();
}

void keithley2400Port::updateValues()
{
    checkInTimeCount();
    if( _expectedAnswer.size() != 0 )
    {
        _expectedAnswer.clear();
        if( !_closing )
        {
            emit portError( "Answer missing!" );
        }
    }

    getResistance();
}

void keithley2400Port::setEmitResistance( bool emitResistance )
{
    _emitResistance = emitResistance;
}

void keithley2400Port::setCls()
{
    sendKeithleyCmd( "*RST", false );
    sendKeithleyCmd( "*CLS", false );
}

void keithley2400Port::setRemoteControl( bool on )
{
    sendKeithleyCmd( QString( "SYST:REM " ) + (on ? "ON" : "OFF"), false );
}

void keithley2400Port::setOutput( bool on )
{
    sendKeithleyCmd( QString( "OUTP " ) + (on ? "ON" : "OFF"), false );
}

void keithley2400Port::setConfigResistance()
{
    sendKeithleyCmd( "CONF:RES", false );
}

void keithley2400Port::getIdn()
{
    sendKeithleyCmd( CMD_IDN, true );
}

QString keithley2400Port::idString() const
{
    return _idString;
}

void keithley2400Port::getResistance()
{
    sendKeithleyCmd( CMD_RESISTANCE, true );
}

void keithley2400Port::sendKeithleyCmd( QString cmd, bool answer )
{
    if( answer )
    {
        if( cmd != CMD_IDN
                && cmd != CMD_RESISTANCE )
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

void keithley2400Port::nextMsg()
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

void keithley2400Port::noAnswerReceived()
{
    if( _msgToSend.size() == 0 )
    {
        return;
    }

    if( _sendCounter < TRIES_SEND_MSG )
    {
        _sendCounter++;
        sendMsg( _msgToSend[0].toStdString().c_str(), _msgToSend[0].size(),
                 false );
        _checkForAnswerTimer.start( _writingPauseMs*2 );
    }
    else
    {
        if( !_closing )
        {
            emit portError( "No answer after several requests!" );
        }
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

void keithley2400Port::sendNextMessage()
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
    else
    {
        _checkForAnswerTimer.stop();
    }
}

void keithley2400Port::receivedMsg( QByteArray msg )
{
    if( msg.isNull() )
    {
        return;
    }
    if( msg.isEmpty() )
    {
        return;
    }

    if( _expectedAnswer.size() == 0 )
    {
        if( _sendCounter != 0 )
        {
            return;
        }
        emit portError( "Unexpected answer!" );
        return;
    }

    _answerPending = 0;
    bool conversionSuccessful = false;
    if( _expectedAnswer[0] == CMD_RESISTANCE )
    {
        _inTimeValueCounter++;
        int start = msg.indexOf( "," );
        start++;
        start = msg.indexOf( ",", start );
        start++;
        int end = msg.indexOf( ",", start );
        double resistance = msg.mid( start, end-start).toDouble(
                    &conversionSuccessful );
        if( conversionSuccessful )
        {
            _lastResistance = resistance;
            emit newResistance( _lastResistance );
        }
    }
    else if( _expectedAnswer[0] == CMD_IDN )
    {
        _idString = msg.left( 20 );
        _initValueCounter++;
        emit initSuccessful( _idString );
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
