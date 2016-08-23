#ifndef THERMOCOUPLEPORT_H
#define THERMOCOUPLEPORT_H

#include <QObject>

#include "easylogging++.h"
#include "labport.h"

class thermocouplePort : public labPort
{
    Q_OBJECT

public:
    explicit thermocouplePort( QObject *parent = 0 );
    void updateValues() override;
    void setEmitProbeTemperature( bool emitProbeTemperature );
    void setEmitAmbientTemperature( bool emitAmbientTemperature );
    QString idString() const;

signals:
    void newProbeTemperature( double temperatureCelsius );
    void newAmbientTemperature( double temperatureCelsius );

private slots:
    void receivedMsg( QByteArray msg );
    void nextMsg();

private:
    void setSerialValues();
    void setLabPortVariables();
    void getId();
    void getInitValues() override;
    void getTemperatures();
    void sendUtcCmd( QString cmd, int numAnswers );
    void noAnswerReceived();
    void sendNextMessage();
    double fahrenheitToCelsius( double temperatureFahrentheit );

    std::vector<QString> _msgToSend;
    std::vector<QString> _expectedAnswer;
    QTimer  _checkForAnswerTimer;
    QString _idString;

    int _sendCounter;
    int _answerPending;
    bool _emitProbeTemperature;
    bool _emitAmbientTemperature;
    bool _idStringSet;
    bool _probeTemperatureOnly;

    const int TRIES_SEND_MSG  = 3;
    const int MAX_TEMPERATURE = 1000;

    const QString CMD_ID                = "ENQ";
    const QString CMD_BOTH_TEMPERATURES = "PA";
    const QString CMD_PROBE_TEMPERATURE = "C";
    const QString CMD_OTHER             = "other";
};

#endif // THERMOCOUPLEPORT_H
