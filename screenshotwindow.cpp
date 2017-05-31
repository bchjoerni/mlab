#include "screenshotwindow.h"
#include "ui_screenshotwindow.h"

screenshotWindow::screenshotWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::screenshotWindow )
{
    _ui->setupUi( this );

    connect( _ui->btn_selectFile, SIGNAL( clicked() ), this,
             SLOT( selectFile() ) );
    connect( _ui->btn_screenshot, SIGNAL( clicked() ), this,
             SLOT( screenshot() ) );
}

screenshotWindow::~screenshotWindow()
{
    delete _ui;
}

void screenshotWindow::selectFile()
{
    _path = QFileDialog::getExistingDirectory( this, "choose a directory", "" );

    if( !_path.isEmpty() )
    {
        _ui->lbl_fileName->setText( _path );
        _ui->btn_screenshot->setEnabled( true );
    }
    else
    {
        _ui->btn_screenshot->setEnabled( false );
    }
}

void screenshotWindow::screenshot()
{
    QPixmap originalPixmap;
    QScreen *screen = QGuiApplication::primaryScreen();
    if( !screen )
    {
        _ui->lbl_status->setText( "ERROR!" );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        emit newError( this->windowTitle() + ": unable to get screen!" );
        return;
    }
    originalPixmap = screen->grabWindow( 0 );

    QString format = "png";
    QString fileName = _path + "/mlab_screens_" +
        QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh_mm_ss" ) +
            "." + format;
    originalPixmap.save( fileName, format.toLatin1().constData() );
    _ui->lbl_status->setText( "Screenshot saved at " +
            QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
}

void screenshotWindow::mLabSignal( char signal, const QString& cmd )
{
    if( signal == 40 )
    {
        if( !_path.isEmpty() )
        {
            screenshot();
        }
    }
}
