#ifndef SCRATCHPADWINDOW_H
#define SCRATCHPADWINDOW_H

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
    class scratchpadWindow;
}

class scratchpadWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit scratchpadWindow( QWidget *parent = 0 );
    ~scratchpadWindow();
    void doUpdate() override
    {
    }
    bool isReceiver() const override
    {
        return false;
    }

private slots:
    void selectFile();
    void save();

private:
    Ui::scratchpadWindow *_ui;
    QString _fileName;
    std::fstream _fileStream;
};

#endif // SCRATCHPADWINDOW_H
