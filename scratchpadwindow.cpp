#include "scratchpadwindow.h"
#include "ui_scratchpadwindow.h"

scratchpadWindow::scratchpadWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::scratchpadWindow )
{
    _ui->setupUi( this );

    connect( _ui->btn_selectFile, SIGNAL( clicked() ), this,
             SLOT( selectFile() ) );
    connect( _ui->btn_save, SIGNAL( clicked() ), this, SLOT( save() ) );
}

scratchpadWindow::~scratchpadWindow()
{
    delete _ui;
}

void scratchpadWindow::selectFile()
{
    QString name = "mlab_notes_" +
        QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh_mm_ss" ) + ".txt";
    _fileName = QFileDialog::getSaveFileName( this, "Select file", name,
                                              "text files (*.txt)" );
    if( !_fileName.isEmpty() )
    {
        _ui->lbl_fileName->setText( _fileName );
        _ui->btn_save->setEnabled( true );
    }
    else
    {
        _ui->btn_save->setEnabled( false );
    }
}

void scratchpadWindow::save()
{
    if( _ui->txt_notes->toPlainText().isEmpty() )
    {
        _ui->lbl_status->setText( "No text written in the scratchpad!" );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        return;
    }

    _fileStream.open( _fileName.toStdString().c_str(), std::ios_base::out );

    if( !_fileStream.is_open() )
    {
        _ui->lbl_status->setText( "ERROR!" );
        _ui->lbl_status->setStyleSheet( STYLE_ERROR );
        return;
    }

    _fileStream << _ui->txt_notes->toPlainText().toStdString() << std::endl;
    _fileStream.close();

    _ui->lbl_status->setText( "Notes saved at " +
            QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) );
    _ui->lbl_status->setStyleSheet( STYLE_OK );
}
