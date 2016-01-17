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
    bool openPort( const QString& portName );
    void sendMsg( const char* msg, int numChars, bool inTime = false );
    void closePort();
    bool isOpen();
    bool isRunning();
    QString getPortName();
    void clearErrors();    
    virtual void updateValues();

signals:
    void portError( QString error );
    void dataReceived( QByteArray msg );

private slots:
    void read();
    void timeToSendMsg();
    void signalError( const QSerialPort::SerialPortError& error );
    void initTimeout();

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

private:    
    QTimer _sendTimer;
    QTimer _initTimer;
    std::vector<QByteArray> _msgToSend;
    char _buffer[512];
};

#endif // LABPORT_H
