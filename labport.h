#ifndef LABPORT_H
#define LABPORT_H

#include <QApplication>
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
    bool openPort( const QString& portName );
    void sendMsg( const char* msg, int numChars, bool inTime = false );
    void closePort();
    bool isOpen();
    bool isRunning();
    QString getPortName();
    void clearPort();
    virtual void updateValues();

    // settings for using enabling using raw labport!:
    void setBaudRate( QSerialPort::BaudRate baudRate );
    void setDataBits( QSerialPort::DataBits dataBits );
    void setStopBits( QSerialPort::StopBits stopBits );
    void setParity( QSerialPort::Parity parity );
    void setFlowControl( QSerialPort::FlowControl flowControl );

signals:
    void portError( QString error );
    void dataReceived( QByteArray msg );

private slots:
    void read();
    void timeToSendMsg();
    void signalError( const QSerialPort::SerialPortError& error );
    void initTimeout();
    void logReadData( const QByteArray& received );

protected:
    virtual void getInitValues();
    void checkInTimeCount();

    QSerialPort _port;
    int _initTimeoutMs;
    int _initValueCounter;
    int _numInitValues;
    int _minBytesRead;
    int _writingPauseMs;
    int _bytesError;
    int _inTimeValueCounter;
    int _numInTimeValues;
    bool _closing;

private:    
    QTimer _sendTimer;
    QTimer _initTimer;
    QTimer _closeTimer;
    std::vector<QByteArray> _msgToSend;
    char _buffer[512];    
};

#endif // LABPORT_H
