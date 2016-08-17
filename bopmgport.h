#ifndef BOPMGPORT_H
#define BOPMGPORT_H

#include <QString>
#include <QTimer>

#include <vector>
#include <string>

#include "easylogging++.h"
#include "labport.h"

class bopmgPort : public labPort
{
    Q_OBJECT

public:
    enum setValueType{ setTypeNone, setTypeVoltage, setTypeCurrent,
                       setTypePowerByVoltage, setTypePowerByCurrent,
                     setTypeResistanceByVoltage, setTypeResistanceByCurrent };

    explicit bopmgPort( QObject *parent = 0 );
    void updateValues() override;
    void setValue( setValueType type, double value, bool autoAdjust = false );
    double minVoltage() const;
    double maxVoltage() const;
    double minCurrent() const;
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
    void nextMsg();

private:
    void setSerialValues();
    void setLabPortVariables();
    void setCls();
    void setOutput( bool on );
    void setRemoteControl( bool on );
    void getIdn();
    void getMinVoltage();
    void getMaxVoltage();
    void getMinCurrent();
    void getMaxCurrent();
    void getInitValues() override;
    void getVoltage();
    void getCurrent();
    double calcAdjustedValue( double setValue, double lastValue );
    void adjustValues();
    void setVoltage( double voltage );
    void setCurrent( double current );
    void sendBopmgCmd( QString cmd, bool answer );
    void noAnswerReceived();
    void sendNextMessage();

    setValueType _setValueType;
    std::vector<QString> _msgToSend;
    std::vector<QString> _expectedAnswer;
    QTimer  _checkForAnswerTimer;
    QString _idString;

    int _sendCounter;
    int _answerPending;
    double _minVoltage;
    double _maxVoltage;
    double _minCurrent;
    double _maxCurrent;
    double _lastVoltage;
    double _lastCurrent;
    double _lastPower;
    double _lastResistance;
    double _setVoltage;
    double _setCurrent;
    double _setPower;
    double _setResistance;
    bool _autoAdjust;
    bool _emitVoltage;
    bool _emitCurrent;
    bool _emitPower;
    bool _emitResistance;

    const int TRIES_SEND_MSG = 3;
    const double adjustmentFactor = 0.3;
    const QString CMD_IDN         = "*IDN?";
    const QString CMD_VOLTAGE_MIN = "VOLT?MIN";
    const QString CMD_VOLTAGE_MAX = "VOLT?MAX";
    const QString CMD_CURRENT_MIN = "CURR?MIN";
    const QString CMD_CURRENT_MAX = "CURR?MAX";
    const QString CMD_VOLTAGE     = "MEAS:VOLT?";
    const QString CMD_CURRENT     = "MEAS:CURR?";
    const QString CMD_OTHER       = "other";
};

#endif // BOPMGPORT_H
