#ifndef WINDOWADDERWINDOW_H
#define WINDOWADDERWINDOW_H

#include <QDialog>

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
    Ui::windowAdderWindow *_ui;
    mdiHandler* _mdi;
};

#endif // WINDOWADDERWINDOW_H
