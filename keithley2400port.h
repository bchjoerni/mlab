#ifndef KEITHLEY2400_H
#define KEITHLEY2400_H

#include <QString>
#include <QTimer>

#include <vector>
#include <string>

#include "easylogging++.h"
#include "labport.h"

class keithley2400Port : public labPort
{
    Q_OBJECT

public:
    explicit keithley2400Port( QObject *parent = 0 );
    void updateValues() override;
    void setEmitResistance( bool emitResistance );
    QString idString() const;

signals:
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
    void setConfigResistance();
    void getResistance();
    void getInitValues() override;
    void sendKeithleyCmd( QString cmd, bool answer );
    void noAnswerReceived();
    void sendNextMessage();

    std::vector<QString> _msgToSend;
    std::vector<QString> _expectedAnswer;
    QTimer  _checkForAnswerTimer;
    QString _idString;

    int _sendCounter;
    int _answerPending;
    double _lastResistance;
    bool _emitResistance;

    const int TRIES_SEND_MSG = 3;
    const QString CMD_IDN         = "*IDN?";
    const QString CMD_RESISTANCE  = "MEAS:RES?";
    const QString CMD_OTHER       = "other";
};

#endif // KEITHLEY2400_H
