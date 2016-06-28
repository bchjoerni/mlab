#ifndef SCREENSHOTWINDOW_H
#define SCREENSHOTWINDOW_H

#include <QString>
#include <QFileDialog>
#include <QDateTime>
#include <QPixmap>
#include <QScreen>


#include "easylogging++.h"
#include "mlabwindow.h"


namespace Ui
{
    class screenshotWindow;
}

class screenshotWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit screenshotWindow( QWidget *parent = 0 );
    ~screenshotWindow();
    void doUpdate() override
    {
    }
    bool isReceiver() const override
    {
        return false;
    }

private slots:
    void selectFile();
    void screenshot();

private:
    Ui::screenshotWindow *_ui;
    QString _path;
};

#endif // SCREENSHOTWINDOW_H
