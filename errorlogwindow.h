#ifndef ERRORLOGWINDOW_H
#define ERRORLOGWINDOW_H

#include <QString>
#include <QFileDialog>
#include <QDateTime>

#include <fstream>
#include <vector>
#include <string>

#include "easylogging++.h"
#include "mlabwindow.h"


namespace Ui
{
    class errorLogWindow;
}

class errorLogWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit errorLogWindow( QWidget *parent = 0 );
    ~errorLogWindow();
    bool isReceiver() const override
    {
        return false;
    }
    void doUpdate() override
    {
    }
    void mLabSignal( char signal, const QString& cmd ) override;
    void mLabError( const QString& errorMsg );

private slots:
    void selectFile();
    void startStopPressed();
    void clearLog();

private:
    Ui::errorLogWindow *_ui;
    QString _fileName;
    std::fstream _fileStream;
    bool _recording;

    const QString START_RECORDING = "start recording";
    const QString STOP_RECORDING  = "stop recording ";

    const QString RECORDING = "recording";
    const QString PAUSING   = "paused   ";
};

#endif // ERRORLOGWINDOW_H
