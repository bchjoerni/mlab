#ifndef WINDOWADDERWINDOW_H
#define WINDOWADDERWINDOW_H

#include <QDialog>
#include <QStringList>
#include <QMessageBox>

#include "easylogging++.h"
#include "mdihandler.h"

namespace Ui
{
    class windowAdderWindow;
}

class windowAdderWindow : public QDialog
{
    Q_OBJECT

public:
    explicit windowAdderWindow( mdiHandler* mdi, QWidget *parent = 0 );
    ~windowAdderWindow();

private slots:
    void ok();
    void cancel();

private:
    bool checkWindowNames();
    bool namesValid( QStringList names );

    Ui::windowAdderWindow *_ui;
    mdiHandler* _mdi;
};

#endif // WINDOWADDERWINDOW_H
