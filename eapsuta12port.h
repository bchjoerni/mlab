#ifndef EAPSUTA12PORT_H
#define EAPSUTA12PORT_H

#include <QObject>
#include <QString>
#include <QApplication>

#include <cmath>

#include "easylogging++.h"
#include "labport.h"

class eapsUta12Port : public labPort
{
    Q_OBJECT

public:
    enum setValueType{ setTypeNone, setTypeVoltage, setTypeCurrent,
                       setTypePowerByVoltage, setTypePowerByCurrent,
                       setTypeResistanceByVoltage, setTypeResistanceByCurrent };

    explicit eapsUta12Port( QObject *parent = 0 );
    void updateValues() override;
    void setValue( setValueType type, double value, bool autoAdjust = false );
    double maxVoltage() const;
    double maxCurrent() const;
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

private slots:
    void receivedMsg( QByteArray msg );

private:
    void setSerialValues();
    void setLabPortVariables();
    void getIdn();
    void getMaxValues();
    void getIdString();
    void setRemoteControl( bool on = true);
    void getInitValues() override;
    void getVoltage();
    void getCurrent();
    double calcAdjustedValue( double setValue, double lastValue );
    void adjustValues();
    void setVoltage( double voltage );
    void setCurrent( double current );
    void setToPrototype( unsigned char* msg );
    void setCheckBytes( unsigned char* msg );
    int  calcCheckBytes( unsigned char* msg );
    bool checkMsgBytes( unsigned char* msg );
    bool checkAnswerFormat( QByteArray msg, unsigned char* msgValues );
    void updateNumInTimeValues();

    void answerSetVoltage();
    void answerGetVoltage( unsigned char* msgValues );
    void answerSetCurrent();
    void answerGetCurrent( unsigned char* msgValues );
    void answerSetStatus();
    void answerGetStatus( unsigned char* msgValues );
    void answerSetIdString();
    void answerGetIdString( QByteArray msg );
    void answerEcho( unsigned char* msgValues );
    void answerError( unsigned char* msgValues );
    void answerSetIdn();
    void answerGetIdn( unsigned char* msgValues );
    void answerGetMaxValues( unsigned char* msgValues );
    void answerSetUserText();
    void answerGetUserText( QByteArray msg );

    QString _idString;
    int _id;
    int _maxVoltageOut;    
    int _maxCurrentOut;
    int _maxVoltageIn;
    int _maxCurrentIn;
    double _minVoltageValue;
    double _maxVoltageValue;
    double _minCurrentValue;
    double _maxCurrentValue;
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
    const static int MESSAGE_LENGTH = 24;
    const unsigned char MESSAGE_PROTOTYPE[MESSAGE_LENGTH]
            = { 170, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
};

#endif // EAPSUTA12PORT_H
