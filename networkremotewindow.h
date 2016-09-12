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
    class networkRemoteWindow;
}

class networkRemoteWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit networkRemoteWindow( QWidget *parent = 0 );
    ~networkRemoteWindow();
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
    void networkFinished( QNetworkReply *reply );
    void commandTicksChanged();
    void readCommands();

private:
    void uploadScreen();
    void downloadCmd();

    Ui::networkRemoteWindow *_ui;
    QNetworkAccessManager* _networkManager;
    QNetworkReply* _networkReply;
    QImage _screen;
    int _counterCommands;
    int _counterScreen;
    QString _password;

    const QString START_REMOTE= "start";
    const QString STOP_REMOTE  = "stop ";

    const QString RUNNING = "running";
    const QString PAUSING = "pausing";
};

#endif // NETWORKREMOTEWINDOW_H