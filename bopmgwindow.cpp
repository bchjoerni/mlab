#include "bopmgwindow.h"
#include "ui_bopmgwindow.h"

bopMgWindow::bopMgWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::bopMgWindow )
{
    _ui->setupUi( this );
}

bopMgWindow::~bopMgWindow()
{
    delete _ui;
}

void bopMgWindow::doUpdate()
{

}
