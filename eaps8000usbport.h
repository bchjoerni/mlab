#ifndef EAPS8000USBPORT_H
#define EAPS8000USBPORT_H

#include <QObject>
#include <QString>
#include <QApplication>

#include <cmath>

#include "easylogging++.h"
#include "labport.h"
#include "byteconverter.h"

class eaps8000UsbPort : public labPort
{
    Q_OBJECT

public:
    enum setValueType{ setTypeNone, setTypeVoltage, setTypeCurrent,
                       setTypePower, setTypeResistanceByVoltage,
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
    QString idString() const;

signals:
    void newVoltage( double volt );
    void newCurrent( double current );
    void newPower( double power );
    void newResistance( double resistance );
    void initSuccessful( QString idString );

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

    void answerDeviceType();
    void answerDeviceSerial();
    void answerNominalVoltage();
    void answerNominalCurrent();
    void answerNominalPower();
    void answerArticleNumber();
    void answerUserText();
    void answerManufacturer();
    void answerSoftwareVersion();
    void answerInterfaceType();
    void answerInterfaceSerial();
    void answerInterfaceArticleNumber();
    void answerInterfaceFirmwareVersion();
    void answerDeviceClass();
    void answerSetVoltage();
    void answerSetCurrent();
    void answerSetPower();
    void answerDeviceState();
    void answerActualValues();
    void answerSetValues();
    void unhandledAnswer();

    QString _idString;
    QByteArray _bufferReceived;
    bool _inMsg;
    int _id;
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
