/**
 * This file is part of the _SimpleMessenger_ project
 *
 * @author Alexandre Bintz <alexandre.bintz@gmail.com>
 * @date   June 2014
 *
 * @file   mainwindow.cpp
 * @brief  Program's main window implementation file
 */

#include "mainwindow.h"
#include <QMessageBox>
#include <QDataStream>
#include <QDateTime>

/**
 * @brief Constructor
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);

    initUI();
    initCore();
}

/**
 * @brief Shows "About SimpleMessenger" popup.
 */
void MainWindow::about()
{
    mAboutDialog->show();
}

/**
 * @brief append a new chat message from local user in the chat box
 * @param sender name of the sender of the message
 * @param message
 */
void MainWindow::appendChatMessageFromLocal(QString message)
{
    appendLineInChatBox("You say: "+message,false,true,true);

    mLastChatBoxLineIsChatMessage = true;
}

/**
 * @brief append a new chat message from the peer in the chat box
 * @param sender name of the sender of the message
 * @param message
 */
void MainWindow::appendChatMessageFromPeer(QString message)
{
    appendLineInChatBox("Peer says: "+message,false,false,true);

    mLastChatBoxLineIsChatMessage = true;
}

/**
 * @brief append a generic line in the chat box
 * @param line
 * @param italic
 * @param color
 * @param doubleSpacing
 */
void MainWindow::appendLineInChatBox(QString line, bool italic, bool bold,QColor color, bool doubleSpacing)
{
    QTextCursor c = ui.textEdit->textCursor();
    c.movePosition(QTextCursor::End);
    ui.textEdit->setTextCursor(c);

    ui.textEdit->insertPlainText(doubleSpacing ? "\n" : "");
    ui.textEdit->insertPlainText(QDateTime::currentDateTime().toString(mDateFormat));
    ui.textEdit->insertPlainText(" ");

    QColor defaultColor = ui.textEdit->textColor();
    ui.textEdit->setTextColor(color);
    ui.textEdit->setFontItalic(italic);
    ui.textEdit->setFontWeight(bold ? QFont::Bold : QFont::Normal);

    ui.textEdit->insertPlainText(line);

    ui.textEdit->setFontWeight(QFont::Normal);
    ui.textEdit->setFontItalic(false);
    ui.textEdit->setTextColor(defaultColor);

    ui.textEdit->insertPlainText("\n");
}

/**
 * @brief append a generic line in the chat box with default color
 * @param line
 * @param italic
 * @param doubleSpacing
 */
void MainWindow::appendLineInChatBox(QString line, bool italic, bool bold, bool doubleSpacing)
{
    appendLineInChatBox(line,italic,bold,ui.textEdit->textColor(),doubleSpacing);
}

/**
 * @brief Attempts connection to host
 */
void MainWindow::clientConnect()
{
    showInfo("Connecting to host...");

    mTcpSocket = new QTcpSocket(this);

    connect(mTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(socketError(QAbstractSocket::SocketError)));
    connect(mTcpSocket,SIGNAL(hostFound()),
            this,SLOT(socketHostFound()));
    connect(mTcpSocket,SIGNAL(connected()),
            this,SLOT(socketConnected()));
    connect(mTcpSocket,SIGNAL(disconnected()),
            this,SLOT(socketDisconnected()));
    connect(mTcpSocket,SIGNAL(readyRead()),
            this,SLOT(socketReadyRead()));

    mTcpSocket->connectToHost(ui.lineEditClientHost->text(),ui.spinBoxClientPort->value());
}

/**
 * @brief Updates GUI to reflect client/server mode change
 */
void MainWindow::clientServerModeChanged()
{
    bool clientMode = ui.radioButtonClientMode->isChecked();
    bool serverMode = ui.radioButtonServerMode->isChecked();

    ui.labelClient->setEnabled(clientMode);
    ui.labelClientHost->setEnabled(clientMode);
    ui.labelClientPort->setEnabled(clientMode);
    ui.lineEditClientHost->setEnabled(clientMode);
    ui.spinBoxClientPort->setEnabled(clientMode);
    ui.pushButtonClientConnect->setEnabled(clientMode);

    ui.labelServer->setEnabled(serverMode);
    ui.labelServerPort->setEnabled(serverMode);
    ui.spinBoxServerPort->setEnabled(serverMode);
    ui.pushButtonServerStart->setEnabled(serverMode);
}

/**
 * @brief Init core data
 */
void MainWindow::initCore()
{
    mTcpServer = 0;
    mTcpSocket = 0;

    mDateFormat = "dd.MM.yy HH:mm";
}

/**
 * @brief Initializes the UI.
 * Should be called only once.
 */
void MainWindow::initUI()
{
    mAboutDialog = new QDialog(this);
    ui_about.setupUi(mAboutDialog);

    mStatusBar = statusBar();

    connect(ui.actionAboutSimpleMessenger,SIGNAL(triggered()),
            this,SLOT(about()));
    connect(ui.actionAboutQt,SIGNAL(triggered()),
            qApp,SLOT(aboutQt()));

    connect(ui.radioButtonClientMode,SIGNAL(toggled(bool)),
            this,SLOT(clientServerModeChanged()));
    connect(ui.radioButtonClientMode,SIGNAL(toggled(bool)),
            this,SLOT(clientServerModeChanged()));

    connect(ui.pushButtonClientConnect,SIGNAL(clicked()),
            this,SLOT(clientConnect()));
    connect(ui.pushButtonServerStart,SIGNAL(clicked()),
            this,SLOT(serverStart()));

    connect(ui.pushButtonMessageSend,SIGNAL(clicked()),
            this,SLOT(messageSend()));
    connect(ui.lineEditMessageSend,SIGNAL(returnPressed()),
            this,SLOT(messageSend()));

    setChatEnabled(false);

    ui.radioButtonClientMode->setChecked(false);
}

/**
 * @brief Send user's message to peer
 */
void MainWindow::messageSend()
{
    if(!mTcpSocket)
        return;

    QString msg = ui.lineEditMessageSend->text();

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2); // prevent errors due to version differences

    out << (quint16)0;  // reserve space for block size
    out << msg;

    out.device()->seek(0);  // go back a the beginning...
    out << ((quint16)(block.size() - sizeof(quint16)));   // ...to write the actual block size

    mTcpSocket->write(block);
    mTcpSocket->flush();

    ui.lineEditMessageSend->clear();
    appendChatMessageFromLocal(msg);

    qDebug() << "BlockSize: " << block.size();
}

/**
 * @brief Handles an incoming connection
 */
void MainWindow::newConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();
    if(mTcpSocket==0)
        return;

    connect(mTcpSocket,SIGNAL(readyRead()),
            this,SLOT(socketReadyRead()));

    showInfo("Client connected from "+mTcpSocket->peerAddress().toString()+":"+QString::number(mTcpSocket->peerPort()));

    setChatEnabled(true);
}

/**
 * @brief shows an error message to the user
 * @param error
 */
void MainWindow::showError(QString error)
{
    appendLineInChatBox(error,true,false,Qt::red,mLastChatBoxLineIsChatMessage);

    mStatusBar->showMessage("Error");
    mLastChatBoxLineIsChatMessage = false;
}

/**
 * @brief shows an info message to the user
 * @param message
 */
void MainWindow::showInfo(QString message)
{
    appendLineInChatBox(message,true,false,mLastChatBoxLineIsChatMessage);

    mStatusBar->showMessage(message);
    mLastChatBoxLineIsChatMessage = false;
}

/**
 * @brief Displays an error message upon server accept error
 * @param error
 */
void MainWindow::serverAcceptError(QAbstractSocket::SocketError error)
{
    showError(mTcpServer->errorString());
}

/**
 * @brief Starts the server
 */
void MainWindow::serverStart()
{
    if(!ui.radioButtonServerMode->isChecked())
        return;

    showInfo("Starting server...");

    int port = ui.spinBoxServerPort->value();

    mTcpServer = new QTcpServer(this);

    connect(mTcpServer,SIGNAL(newConnection()),
            this,SLOT(newConnection()));
    connect(mTcpServer,SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this,SLOT(serverAcceptError(QAbstractSocket::SocketError)));

    if(mTcpServer->listen(QHostAddress::Any,port))
    {
        ui.spinBoxServerPort->setValue(mTcpServer->serverPort());
        showInfo("Server is listening on port "+QString::number(mTcpServer->serverPort()));
    }
    else
    {
        showError(mTcpServer->errorString());
    }
}

/**
 * @brief En/disables widget for sending chat message
 * @param enabled
 */
void MainWindow::setChatEnabled(bool enabled)
{
    ui.lineEditMessageSend->setEnabled(enabled);
    ui.pushButtonMessageSend->setEnabled(enabled);
}

/**
 * @brief Displays error message upon socket error
 * @param error
 */
void MainWindow::socketError(QAbstractSocket::SocketError error)
{
    showError(mTcpSocket->errorString());
}

/**
 * @brief Client socket is connected
 */
void MainWindow::socketConnected()
{
    showInfo("Connected");

    setChatEnabled(true);
}

/**
 * @brief Client socket got disconnected
 */
void MainWindow::socketDisconnected()
{
    showInfo("Disconnected");

    setChatEnabled(false);
}

/**
 * @brief Reacts to socket::hostFound signal
 */
void MainWindow::socketHostFound()
{
    showInfo("Host found");
}

/**
 * @brief Data are available for reading on the socket
 */
void MainWindow::socketReadyRead()
{
    qDebug() << "ReadyRead";

    QDataStream in(mTcpSocket);
    in.setVersion(QDataStream::Qt_5_2); // prevent errors due to version differences

    static quint16 blockSize=0;
    QString msg;

    qDebug() << "BlockSize: " << blockSize;
    qDebug() << "bytesAvailable: " << mTcpSocket->bytesAvailable();

    if(blockSize==0)
    {
        if(mTcpSocket->bytesAvailable() < (qint64)sizeof(quint16))
            return;

        in >> blockSize;
        qDebug() << "Got BlockSize: " << blockSize;
    }

    if(mTcpSocket->bytesAvailable() < blockSize)
        return;

    in >> msg;
    qDebug() << "Got Message: " << msg;

    blockSize = 0;

    appendChatMessageFromPeer(msg);
}


