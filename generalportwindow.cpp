#include "generalportwindow.h"
#include "ui_generalportwindow.h"

generalPortWindow::generalPortWindow( QWidget *parent ) :
    mLabWindow( parent ),
    _ui( new Ui::generalPortWindow )
{
    _ui->setupUi( this );

    addPortSettingItems();
    refreshPortList();

    connectPortFunctions();
    connectUiElements();
}

generalPortWindow::~generalPortWindow()
{
    delete _ui;
}

void generalPortWindow::connectPortFunctions()
{
    connect( &_port, SIGNAL( portError( QString ) ), this,
             SLOT( portError( QString ) ) );
    connect( &_port, SIGNAL( dataReceived( QByteArray ) ), this,
             SLOT( receivedMsg( QByteArray ) ) );
}

void generalPortWindow::connectUiElements()
{
    connect( _ui->btn_clearOutput, SIGNAL( clicked() ), this,
             SLOT( clearOutput() ) );
    connect( _ui->btn_copyOutputToClipboard, SIGNAL( clicked() ), this,
             SLOT( copyOutputToClipboard() ) );
    connect( _ui->btn_portSettingsInfo, SIGNAL( clicked() ), this,
             SLOT( showPortSettingsInfo() ) );
    connect( _ui->btn_selectedPortInfo, SIGNAL( clicked() ), this,
             SLOT( showInfoSelectedPort() ) );
    connect( _ui->btn_showInfoAllPorts, SIGNAL( clicked() ), this,
             SLOT( showInfoAllPorts() ) );
    connect( _ui->btn_connect, SIGNAL( clicked() ), this,
             SLOT( connectivityButtonPressed() ) );
    connect( _ui->btn_resetInfo, SIGNAL( clicked() ), this,
             SLOT( resetInfo() ) );
    connect( _ui->btn_send, SIGNAL( clicked() ), this, SLOT( sendMsg() ) );
    connect( _ui->btn_displayMsg, SIGNAL( clicked() ), this,
             SLOT( showReceivedMsg() ) );
}

void generalPortWindow::addPortSettingItems()
{
    addBaudRateItems();
    addDataBitsItems();
    addParityItems();
    addStopBitItems();
    addFlowControlItems();
}

void generalPortWindow::addBaudRateItems()
{
    _ui->cob_baudRate->addItem( PORT_BAUD_1200 );
    _ui->cob_baudRate->addItem( PORT_BAUD_2400 );
    _ui->cob_baudRate->addItem( PORT_BAUD_4800 );
    _ui->cob_baudRate->addItem( PORT_BAUD_9600 );
    _ui->cob_baudRate->addItem( PORT_BAUD_19200 );
    _ui->cob_baudRate->addItem( PORT_BAUD_38400 );
    _ui->cob_baudRate->addItem( PORT_BAUD_57600 );
    _ui->cob_baudRate->addItem( PORT_BAUD_115200 );

    _ui->cob_baudRate->setCurrentIndex( 5 );
}

void generalPortWindow::addDataBitsItems()
{
    _ui->cob_dataBits->addItem( PORT_DATABITS_5 );
    _ui->cob_dataBits->addItem( PORT_DATABITS_6 );
    _ui->cob_dataBits->addItem( PORT_DATABITS_7 );
    _ui->cob_dataBits->addItem( PORT_DATABITS_8 );

    _ui->cob_dataBits->setCurrentIndex( 3 );
}

void generalPortWindow::addParityItems()
{
    _ui->cob_parity->addItem( PORT_PARITY_NONE );
    _ui->cob_parity->addItem( PORT_PARITY_ODD );
    _ui->cob_parity->addItem( PORT_PARITY_EVEN );
    _ui->cob_parity->addItem( PORT_PARITY_SPACE );
    _ui->cob_parity->addItem( PORT_PARITY_MARK );

    _ui->cob_parity->setCurrentIndex( 0 );
}

void generalPortWindow::addStopBitItems()
{
    _ui->cob_stopBits->addItem( PORT_STOPBITS_ONE );
    _ui->cob_stopBits->addItem( PORT_STOPBITS_ONEANDHALF );
    _ui->cob_stopBits->addItem( PORT_STOPBITS_TWO );

    _ui->cob_stopBits->setCurrentIndex( 0 );
}

void generalPortWindow::addFlowControlItems()
{
    _ui->cob_flowControl->addItem( PORT_FLOWCONTROL_NONE );
    _ui->cob_flowControl->addItem( PORT_FLOWCONTROL_HARDWARE );
    _ui->cob_flowControl->addItem( PORT_FLOWCONTROL_SOFTWARE );

    _ui->cob_flowControl->setCurrentIndex( 0 );
}

void generalPortWindow::appendText( const QString& text )
{
    _ui->txt_output->append( (_ui->chb_showTime->isChecked() ?
            QTime::currentTime().toString( "hh:mm:ss.zzz: " ) : "") + text );
}

void generalPortWindow::refreshPortList()
{
    _ui->cob_ports->clear();

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        if( !info.isBusy() )
        {
            _ui->cob_ports->addItem( info.portName() );
        }
    }

    _ui->cob_ports->setCurrentIndex( 0 );
    _ui->btn_connect->setEnabled( _ui->cob_ports->count() > 0 );
    _ui->cob_ports->setEnabled( _ui->cob_ports->count() > 0 );

    if( _ui->cob_ports->count() < 1 )
    {
        _ui->cob_ports->addItem( NO_PORTS_AVAILABLE );
    }
}

QString generalPortWindow::getPortInfoText( const QString& portName )
{
    QSerialPortInfo info( portName );

    return "Port: " + info.portName() + "\n"
           "Location: " + info.systemLocation() + "\n"
           "Description: " + info.description() + "\n"
           "Manufacturer: " + info.manufacturer() + "\n"
           "Serial number: " + info.serialNumber() + "\n"
           "Vendor Identifier: " +
                (info.hasVendorIdentifier() ? QString::number(
                    info.vendorIdentifier(), 16 ) : "-") + "\n"
           "Product Identifier: " +
                (info.hasProductIdentifier() ? QString::number(
                    info.productIdentifier(), 16 ) : "-") + "\n"
           "Busy: " + (info.isBusy() ? "Yes" : "No") + "\n";
}

void generalPortWindow::showPortSettingsInfo()
{
    appendText( "Muss noch geschrieben werden!" );
}

void generalPortWindow::showInfoSelectedPort()
{
    QString itemText = _ui->cob_ports->currentText();
    if( !itemText.isEmpty()
            && itemText != NO_PORTS_AVAILABLE )
    {
        appendText( "port info:\n" + getPortInfoText( itemText ) );
    }
}

void generalPortWindow::showInfoAllPorts()
{
    QString portInfos;

    foreach( const QSerialPortInfo &info, QSerialPortInfo::availablePorts() )
    {
        portInfos += getPortInfoText( info.portName() ) + "\n";
    }

    if( portInfos.isNull() || portInfos.isEmpty() )
    {
        portInfos = "No ports available!";
    }

    appendText( portInfos );
}

void generalPortWindow::copyOutputToClipboard()
{
    _ui->txt_output->selectAll();
    _ui->txt_output->copy();
}

void generalPortWindow::clearOutput()
{
    _ui->txt_output->clear();
}

bool generalPortWindow::setBaudrate()
{
    if( _ui->cob_baudRate->currentText() == PORT_BAUD_1200 )
    {
        _port.setBaudRate( QSerialPort::Baud1200 );
    }
    else if( _ui->cob_baudRate->currentText() == PORT_BAUD_2400 )
    {
        _port.setBaudRate( QSerialPort::Baud2400 );
    }
    else if( _ui->cob_baudRate->currentText() == PORT_BAUD_4800 )
    {
        _port.setBaudRate( QSerialPort::Baud4800 );
    }
    else if( _ui->cob_baudRate->currentText() == PORT_BAUD_9600 )
    {
        _port.setBaudRate( QSerialPort::Baud9600 );
    }
    else if( _ui->cob_baudRate->currentText() == PORT_BAUD_19200 )
    {
        _port.setBaudRate( QSerialPort::Baud19200 );
    }
    else if( _ui->cob_baudRate->currentText() == PORT_BAUD_38400 )
    {
        _port.setBaudRate( QSerialPort::Baud38400 );
    }
    else if( _ui->cob_baudRate->currentText() == PORT_BAUD_57600 )
    {
        _port.setBaudRate( QSerialPort::Baud57600 );
    }
    else if( _ui->cob_baudRate->currentText() == PORT_BAUD_115200 )
    {
        _port.setBaudRate( QSerialPort::Baud115200 );
    }
    else
    {
        portError( "Unknown baud rate!" );
        return false;
    }
    return true;
}

bool generalPortWindow::setDataBits()
{
    if( _ui->cob_dataBits->currentText() == PORT_DATABITS_5 )
    {
        _port.setDataBits( QSerialPort::Data5 );
    }
    else if( _ui->cob_dataBits->currentText() == PORT_DATABITS_6 )
    {
        _port.setDataBits( QSerialPort::Data6 );
    }
    else if( _ui->cob_dataBits->currentText() == PORT_DATABITS_7 )
    {
        _port.setDataBits( QSerialPort::Data7 );
    }
    else if( _ui->cob_dataBits->currentText() == PORT_DATABITS_8 )
    {
        _port.setDataBits( QSerialPort::Data8 );
    }
    else
    {
        portError( "Unknown data bits!" );
        return false;
    }
    return true;
}

bool generalPortWindow::setParity()
{
    if( _ui->cob_parity->currentText() == PORT_PARITY_NONE )
    {
        _port.setParity( QSerialPort::NoParity );
    }
    else if( _ui->cob_parity->currentText() == PORT_PARITY_ODD )
    {
        _port.setParity( QSerialPort::OddParity );
    }
    else if( _ui->cob_parity->currentText() == PORT_PARITY_EVEN )
    {
        _port.setParity( QSerialPort::EvenParity );
    }
    else if( _ui->cob_parity->currentText() == PORT_PARITY_SPACE )
    {
        _port.setParity( QSerialPort::SpaceParity );
    }
    else if( _ui->cob_parity->currentText() == PORT_PARITY_MARK )
    {
        _port.setParity( QSerialPort::MarkParity );
    }
    else
    {
        portError( "Unknown parity!" );
        return false;
    }
    return true;
}

bool generalPortWindow::setStopBits()
{
    if( _ui->cob_stopBits->currentText() == PORT_STOPBITS_ONE )
    {
        _port.setStopBits( QSerialPort::OneStop );
    }
    else if( _ui->cob_stopBits->currentText() == PORT_STOPBITS_ONEANDHALF )
    {
        _port.setStopBits( QSerialPort::OneAndHalfStop );
    }
    else if( _ui->cob_stopBits->currentText() == PORT_STOPBITS_TWO )
    {
        _port.setStopBits( QSerialPort::TwoStop );
    }
    else
    {
        portError( "Unknown stop bits!" );
        return false;
    }
    return true;
}

bool generalPortWindow::setFlowControl()
{
    if( _ui->cob_flowControl->currentText() == PORT_FLOWCONTROL_NONE )
    {
        _port.setFlowControl( QSerialPort::NoFlowControl );
    }
    else if( _ui->cob_flowControl->currentText() == PORT_FLOWCONTROL_HARDWARE )
    {
        _port.setFlowControl( QSerialPort::HardwareControl );
    }
    else if( _ui->cob_flowControl->currentText() == PORT_FLOWCONTROL_SOFTWARE )
    {
        _port.setFlowControl( QSerialPort::SoftwareControl );
    }
    else
    {
        portError( "Unknown flow control!" );
        return false;
    }
    return true;
}

void generalPortWindow::connectivityButtonPressed()
{
    if( _ui->btn_connect->text() == CONNECT_PORT )
    {
        connectPort();
    }
    else if( _ui->btn_connect->text() == DISCONNECT_PORT )
    {
        disconnectPort();
    }
}

void generalPortWindow::connectPort()
{
    if( !setBaudrate()
            || !setDataBits()
            || !setParity()
            || !setStopBits()
            || !setFlowControl() )
    {
        return;
    }

    if( _port.openPort( _ui->cob_ports->currentText() ) )
    {
        _ui->lbl_status->setText( CONNECTED );
        _ui->lbl_status->setStyleSheet( STYLE_OK );
        _ui->btn_connect->setText( DISCONNECT_PORT );
    }
}

void generalPortWindow::disconnectPort()
{
    _port.closePort();

    _ui->lbl_status->setText( NOT_CONNECTED );
    _ui->lbl_status->setStyleSheet( STYLE_ERROR );
    _ui->btn_connect->setText( CONNECT_PORT );
}

void generalPortWindow::portError( QString error )
{
    LOG(INFO) << this->windowTitle().toStdString() << ": port error: "
              << error.toStdString();
    _ui->lbl_info->setText( error );
    _ui->lbl_info->setStyleSheet( STYLE_ERROR );
}

void generalPortWindow::resetInfo()
{
    _port.clearPort();

    if( _port.isRunning() )
    {
        _ui->lbl_info->setStyleSheet( "" );
        _ui->btn_connect->setText( DISCONNECT_PORT );
        _ui->btn_connect->setEnabled( true );
    }
    else
    {
        _ui->lbl_info->setText( "-" );
        _ui->lbl_info->setStyleSheet( "" );
        _ui->btn_connect->setText( CONNECT_PORT );
        refreshPortList();
    }
}

void generalPortWindow::receivedMsg( QByteArray data )
{
    if( _ui->rad_displayReceivedOnClick->isChecked() )
    {
        _receivedMsgBuffer.append( data );
    }
    else
    {
        decodeAndDisplayReceived( data );
    }
}

void generalPortWindow::decodeAndDisplayReceived( const QByteArray& data )
{
    std::string decMsg;
    std::string hexMsg;
    for( int i = 0; i < data.size(); i++ )
    {
        char charVal = data.at( i );
        unsigned char uc = (*((unsigned char*) &charVal));
        decMsg += std::to_string( uc ) + " ";
        hexMsg += byteConverter::uCharToReadableHex( uc ) + " ";
    }

    QString receivedString = "Received message!";

    if( _ui->chb_showString->isChecked() )
    {
        receivedString += "\n\t string: " + data;
    }
    if( _ui->chb_showDec->isChecked() )
    {
        receivedString += "\n\t dec: " + QString::fromStdString( decMsg );
    }
    if( _ui->chb_showHex->isChecked() )
    {
        receivedString += "\n\t hex: " + QString::fromStdString( hexMsg );
    }

    appendText( receivedString );
}

void generalPortWindow::showReceivedMsg()
{
    if( _receivedMsgBuffer.isEmpty() )
    {
        appendText( "Nothing new received yet!" );
    }
    else
    {
        decodeAndDisplayReceived( _receivedMsgBuffer );
    }

    _receivedMsgBuffer.clear();
}

void generalPortWindow::sendMsg()
{
    if( _ui->txt_msgToSend->text().isEmpty() )
    {
        return;
    }

    if( _ui->rad_sendString->isChecked() )
    {
        QString msg = _ui->txt_msgToSend->text()
                + (_ui->chb_appendCRLF->isChecked() ? "\r\n" : "");
        _port.sendMsg( msg.toStdString().c_str(), msg.size(), false );
        appendText( "Send: " + _ui->txt_msgToSend->text()
                    + (_ui->chb_appendCRLF->isChecked() ? "\\r\\n" : ""));
    }
    else
    {
        sendChars();
    }

    _ui->txt_msgToSend->setText( "" );
}

void generalPortWindow::sendChars()
{
    QStringList chars = _ui->txt_msgToSend->text().split( "," );
    std::vector<unsigned char> charVal;
    foreach( QString c, chars )
    {
        bool conversionSuccessful = false;
        unsigned int value;
        if( _ui->rad_sendDec->isChecked() )
        {
            value = c.toUInt( &conversionSuccessful, 10 );
        }
        else if( _ui->rad_sendHex->isChecked() )
        {
            value = c.toUInt( &conversionSuccessful, 16 );
        }
        else
        {
            return;
        }

        if( conversionSuccessful && value < 256 )
        {
            charVal.push_back( static_cast<unsigned char>( value ) );
        }
        else
        {
            appendText( "Unable to send message: Invalid character value: '"
                      + c + "'!" );
            return;
        }
    }

    if( charVal.size() == 0 )
    {
        appendText( "No characters to send!" );
        return;
    }

    char* bytes = new char[charVal.size()];
    for( unsigned int i = 0; i < charVal.size(); i++ )
    {
        unsigned char* ucPointer = &charVal[i];
        bytes[i] = (*((char*) ucPointer));
    }

    _port.sendMsg( bytes, charVal.size(), false );
    appendText( "Send: " + _ui->txt_msgToSend->text() );

    delete bytes;
}


