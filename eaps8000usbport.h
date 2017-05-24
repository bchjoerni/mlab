#ifndef EAPS8000USBPORT_H
#define EAPS8000USBPORT_H

#include <QDebug>

#include <QObject>
#include <QString>
#include <QApplication>

#include "easylogging++.h"
#include "labport.h"
#include "byteconverter.h"

class eaps8000UsbPort : public labPort
{
    Q_OBJECT

public:
    enum setValueType{ setTypeNone, setTypeVoltage, setTypeCurrent,
                       setTypePower, setTypePowerByVoltage,
                       setTypePowerByCurrent, setTypeResistanceByVoltage,
                       setTypeResistanceByCurrent };

    explicit eaps8000UsbPort( QObject *parent = 0 );
    void updateValues() override;
    void setValue( setValueType type, double value, bool autoAdjust = false );
    double maxVoltage() const;
    double maxCurrent() const;
    double maxPower() const;
    void setEmitVoltage( bool emitVoltage );
    void setEmitCurrent( bool emitCurrent );
    void setEmitPower( bool emitPower );
    void setEmitResistance( bool emitResistance );
    void setPowerType( bool isAbleToSetPowerDirectly );
    QString idString() const;

signals:
    void newVoltage( double volt );
    void newCurrent( double current );
    void newPower( double power );
    void newResistance( double resistance );

private slots:
    void receivedMsg( QByteArray msg );

private:
    void setSerialValues();
    void setLabPortVariables();
    void getMaxValues();
    void getMaxVoltage();
    void getMaxCurrent();
    void getMaxPower();
    void getIdString();
    void setRemoteControl( bool on = true );
    void getInitValues() override;
    void getCurrentValues();
    double calcAdjustedValue( double setValue, double lastValue );
    void adjustValues();
    void setVoltage( double voltage );
    void setCurrent( double current );
    void setPower( double power );
    void updateNumInTimeValues();
    int getAnswerLength( unsigned char startDelimiter );
    char getMessageType( unsigned char objectByte );
    int getMessageLength( unsigned char objectByte );
    void interpretMessage( QByteArray msg );
    void sendSetValue( int object, int value );
    void sendGetValue( int object, bool inTime = false );

    void answerDeviceType( const QByteArray& msg );
    void answerDeviceSerial( const QByteArray& msg );
    void answerNominalVoltage( const QByteArray& msg );
    void answerNominalCurrent( const QByteArray& msg );
    void answerNominalPower( const QByteArray& msg );
    void answerArticleNumber( const QByteArray& msg );
    void answerUserText( const QByteArray& msg );
    void answerManufacturer( const QByteArray& msg );
    void answerSoftwareVersion( const QByteArray& msg );
    void answerInterfaceType( const QByteArray& msg );
    void answerInterfaceSerial( const QByteArray& msg );
    void answerInterfaceArticleNumber( const QByteArray& msg );
    void answerInterfaceFirmwareVersion( const QByteArray& msg );
    void answerDeviceClass( const QByteArray& msg );
    void answerSetVoltage( const QByteArray& msg );
    void answerSetCurrent( const QByteArray& msg );
    void answerSetPower( const QByteArray& msg );
    void answerDeviceState( const QByteArray& msg );
    void answerActualValues( const QByteArray& msg );
    void answerSetValues( const QByteArray& msg );
    void answerError( const QByteArray& msg );
    void unhandledAnswer( const QByteArray& msg );

    QString _idString;
    QByteArray _bufferReceived;
    bool _inMsg;
    int _id;
    bool _setPowerDirectly;
    double _maxVoltage;
    double _maxCurrent;
    double _maxPower;
    double _lastVoltage;
    double _lastCurrent;
    double _lastPower;
    double _lastResistance;
    setValueType _setValueType;
    double _setVoltage;
    double _setCurrent;
    double _setPower;
    double _setResistance;
    bool _autoAdjust;
    bool _emitVoltage;
    bool _emitCurrent;
    bool _emitPower;
    bool _emitResistance;

    const double adjustmentFactor = 0.3;
};

#endif // EAPS8000USBPORT_H
