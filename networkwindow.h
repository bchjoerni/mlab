#ifndef NETWORKREMOTEWINDOW_H
#define NETWORKREMOTEWINDOW_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QString>
#include <QBuffer>
#include <QPixmap>
#include <QScreen>

#include "easylogging++.h"
#include "mlabwindow.h"

namespace Ui
{
    class networkWindow;
}

class networkWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit networkWindow( QWidget *parent = 0 );
    ~networkWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }
    void mLabSignal( char signal, const QString& cmd ) override;

private slots:
    void startStopPressed();
    void networkError( QNetworkReply::NetworkError error );
    void uploadProgress( qint64, qint64 );
    void downloadProgress( qint64, qint64 );
    void uploadFinished();
    void commandTicksChanged();
    void readCommands();

private:
    void uploadScreen();
    void downloadCmd();

    Ui::networkWindow *_ui;
    QNetworkAccessManager* _networkManager;
    QNetworkReply* _downloadReply;
    QByteArray _screenBytes;
    int _counterCommands;
    int _counterScreen;
    int _counterReset;
    QString _password;

    const QString START_REMOTE= "start";
    const QString STOP_REMOTE  = "stop ";

    const QString RUNNING = "running";
    const QString PAUSING = "pausing";
};

#endif // NETWORKREMOTEWINDOW_H
