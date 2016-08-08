#ifndef PAIRSAVEWINDOW_H
#define PAIRSAVEWINDOW_H

#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QDateTime>

#include <fstream>

#include "easylogging++.h"
#include "mlabwindow.h"

namespace Ui
{
    class pairSaveWindow;
}

class pairSaveWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit pairSaveWindow( QWidget *parent = 0 );
    ~pairSaveWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return true;
    }
    void putValue( const QString& id, double value ) override;
    void mLabSignal( char signal ) override;

private slots:
    void selectFile();
    void startStopPressed();
    void resetCounter();

private:
    void addDelimiters();
    QString getDelimiter();

    const QString START_RECORDING = "start recording";
    const QString STOP_RECORDING  = "stop recording ";

    const QString RECORDING = "recording";
    const QString PAUSING   = "paused   ";

    const QString DELIMITER_KOMMA      = ", (komma)";
    const QString DELIMITER_SEMIKOLON  = "; (semikolon)";
    const QString DELIMITER_WHITESPACE = "  (whitespace)";
    const QString DELIMITER_TABSTOP    = "  (tabstop)";

    Ui::pairSaveWindow *_ui;
    QString _fileName;
    unsigned int _intervalCounter;
    unsigned int _savedCounter;
    double _x;
    double _y;
    std::fstream _fileStream;
    bool _recording;
};

#endif // PAIRSAVEWINDOW_H
