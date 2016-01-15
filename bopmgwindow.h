#ifndef BOPMGWINDOW_H
#define BOPMGWINDOW_H

#include "easylogging++.h"
#include "mlabwindow.h"

#include <QWidget>

namespace Ui
{
    class bopMgWindow;
}

class bopMgWindow : public mLabWindow
{
    Q_OBJECT

public:
    explicit bopMgWindow( QWidget *parent = 0 );
    ~bopMgWindow();
    void doUpdate() override;
    bool isReceiver() const override
    {
        return false;
    }

private:
    Ui::bopMgWindow *_ui;
};

#endif // BOPMGWINDOW_H
