#ifndef GENERALPORTWINDOW_H
#define GENERALPORTWINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTime>

#include "easylogging++.h"
#include "labport.h"
#include "mlabwindow.h"
#include "byteconverter.h"

namespace Ui
{
    class generalPortWindow;
}

class generalPortWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit generalPortWindow( QWidget *parent = 0 );
    ~generalPortWindow();
    void doUpdate() override
    {
    }
    bool isReceiver() const override
    {
        return false;
    }

private slots:
    void showPortSettingsInfo();
    void showInfoSelectedPort();
    void showInfoAllPorts();
    void copyOutputToClipboard();
    void clearOutput();
    void connectivityButtonPressed();
    void portError( QString error );
    void resetInfo();
    void receivedMsg( QByteArray msg );
    void showReceivedMsg();
    void sendMsg();

private:
    void connectPortFunctions();
    void connectUiElements();
    void addPortSettingItems();
    void addBaudRateItems();
    void addDataBitsItems();
    void addParityItems();
    void addStopBitItems();
    void addFlowControlItems();
    void refreshPortList();
    void appendText( const QString& text );
    QString getPortInfoText( const QString& portName );
    bool setPortName();
    bool setBaudrate();
    bool setDataBits();
    bool setParity();
    bool setStopBits();
    bool setFlowControl();
    void connectPort();
    void disconnectPort();
    void sendChars();
    void decodeAndDisplayReceived( const QByteArray& data );

    Ui::generalPortWindow *_ui;
    labPort _port;
    QByteArray _receivedMsgBuffer;


    const QString PORT_BAUD_1200   = "1200";
    const QString PORT_BAUD_2400   = "2400";
    const QString PORT_BAUD_4800   = "4800";
    const QString PORT_BAUD_9600   = "9600";
    const QString PORT_BAUD_19200  = "19200";
    const QString PORT_BAUD_38400  = "38400";
    const QString PORT_BAUD_57600  = "57600";
    const QString PORT_BAUD_115200 = "115200";

    const QString PORT_DATABITS_5  = "5";
    const QString PORT_DATABITS_6  = "6";
    const QString PORT_DATABITS_7  = "7";
    const QString PORT_DATABITS_8  = "8";

    const QString PORT_PARITY_NONE  = "no parity";
    const QString PORT_PARITY_ODD   = "odd";
    const QString PORT_PARITY_EVEN  = "even";
    const QString PORT_PARITY_SPACE = "space";
    const QString PORT_PARITY_MARK  = "mark";

    const QString PORT_STOPBITS_ONE        = "1";
    const QString PORT_STOPBITS_ONEANDHALF = "1.5";
    const QString PORT_STOPBITS_TWO        = "2";

    const QString PORT_FLOWCONTROL_NONE     = "no flow control";
    const QString PORT_FLOWCONTROL_HARDWARE = "hardware control";
    const QString PORT_FLOWCONTROL_SOFTWARE = "software control";
};

#endif // GENERALPORTWINDOW_H
