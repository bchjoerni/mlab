#ifndef ALLSAVEWINDOW_H
#define ALLSAVEWINDOW_H

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
    class allSaveWindow;
}

class allSaveWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit allSaveWindow( QWidget *parent = 0 );
    ~allSaveWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return true;
    }
    void putValue( const QString& id, double value );

private slots:
    void selectFile();
    void startStopPressed();
    void resetCounter();

private:
    Ui::allSaveWindow *_ui;
    QString _fileName;
    std::fstream _fileStream;
    std::vector<std::string> _data;
    unsigned int _intervalCounter;
    unsigned int _savedCounter;
    bool _recording;

    const QString START_RECORDING = "start recording";
    const QString STOP_RECORDING  = "stop recording ";

    const QString RECORDING = "recording";
    const QString PAUSING   = "paused   ";
};

#endif // ALLSAVEWINDOW_H
