#ifndef SCREENUPLOADERWINDOW_H
#define SCREENUPLOADERWINDOW_H

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
    class screenUploaderWindow;
}

class screenUploaderWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit screenUploaderWindow( QWidget *parent = 0 );
    ~screenUploaderWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }
    void mLabSignal( char signal, const QString& cmd ) override;

private slots:
    void startStopPressed();
    void uploadError( QNetworkReply::NetworkError error );
    void uploadProgress( qint64, qint64 );
    void uploadFinished();
    void updateTickCounter();

private:
    void uploadScreen();

    Ui::screenUploaderWindow *_ui;
    QNetworkAccessManager* _networkManager;
    QByteArray _screenBytes;
    int _counter;
    QString _username;
    QString _password;    

    const QString START_UPLOADING = "start";
    const QString STOP_UPLOADING  = "stop ";

    const QString RUNNING = "running";
    const QString PAUSING = "pausing";
};

#endif // SCREENUPLOADERWINDOW_H
