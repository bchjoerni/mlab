#ifndef LABPORT_H
#define LABPORT_H

#include <QSerialPort>
#include <QTimer>
#include <QByteArray>

#include <vector>
#include <string>
#include <memory>

#include "easylogging++.h"

class labPort : public QObject
{
    Q_OBJECT

public:
    explicit labPort( QObject *parent = 0 );
    void setBaudRate( QSerialPort::BaudRate baudRate );
    void setDataBits( QSerialPort::DataBits dataBits );
    void setStopBits( QSerialPort::StopBits stopBits );
    void setParity(   QSerialPort::Parity parity );
    void setInitTimeoutMs( int initTimeoutMs );
    void setNumInitValues( int numInitValues );
    void setMinBytesRead( int minBytesRead );
    void setWritingPauseMs( int writingPauseMs );
    void setBytesError( int bytesError );
    bool openPort( const QString& portName );
    void sendMsg( char* msg, int numChars, bool realtime = false );
    void closePort();
    bool isOpen();
    void setRunning( bool running );
    bool isRunning();
    QString getPortName();
    void clearErrors();    
    virtual void updateValues();

signals:
    void portError( QString error );
    void dataReceived( QByteArray msg );
    void answerTimeout();

protected slots:
    virtual void read();

private slots:
    void timeToSendMsg();
    void signalError( const QSerialPort::SerialPortError& error );
    void initTimeout();

protected:
    virtual void getInitValues();
    virtual void getUpdateValues();

    int _initValueCounter;
    int _realtimeValueCounter;

private:
    QSerialPort _port;
    QTimer _sendTimer;
    QTimer _initTimer;
    std::vector<std::unique_ptr<char[]> > _msgToSend;
    char _buffer[512];

    int _initTimeoutMs;
    int _numInitValues;
    int _minBytesRead;
    int _writingPauseMs;
    int _bytesError;
};

#endif // LABPORT_H
