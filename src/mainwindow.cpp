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
 * @brief Attempts connection to host
 */
void MainWindow::clientConnect()
{
    mStatusBar->showMessage("Connecting to host...");

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
    out.setVersion(QDataStream::Qt_5_2);

    out << (quint16)0;  // reserve space for block size
    out << msg;

    out.device()->seek(0);  // go back a the beginning...
    out << ((quint16)(block.size() - sizeof(quint16)));   // ...to write the actual block size

    mTcpSocket->write(block);
    mTcpSocket->flush();

    ui.lineEditMessageSend->clear();
    ui.textEdit->append(msg);

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

    mStatusBar->showMessage("Client connected from "+mTcpSocket->peerAddress().toString()+":"+QString::number(mTcpSocket->peerPort()));

    setChatEnabled(true);
}

/**
 * @brief Displays an error message upon server accept error
 * @param error
 */
void MainWindow::serverAcceptError(QAbstractSocket::SocketError error)
{
    QMessageBox::warning(this,"Error",mTcpServer->errorString());
    mStatusBar->clearMessage();
}

/**
 * @brief Starts the server
 */
void MainWindow::serverStart()
{
    if(!ui.radioButtonServerMode->isChecked())
        return;

    mStatusBar->showMessage("Starting server...");

    int port = ui.spinBoxServerPort->value();

    mTcpServer = new QTcpServer(this);

    connect(mTcpServer,SIGNAL(newConnection()),
            this,SLOT(newConnection()));
    connect(mTcpServer,SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this,SLOT(serverAcceptError(QAbstractSocket::SocketError)));

    if(mTcpServer->listen(QHostAddress::Any,port))
    {
        ui.spinBoxServerPort->setValue(mTcpServer->serverPort());
        mStatusBar->showMessage("Server is listening on port "+QString::number(mTcpServer->serverPort()));
    }
    else
    {
        QMessageBox::warning(this,"Error",mTcpServer->errorString());
        mStatusBar->clearMessage();
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
    QMessageBox::warning(this,"Error",mTcpSocket->errorString());
    mStatusBar->clearMessage();
}

/**
 * @brief Client socket is connected
 */
void MainWindow::socketConnected()
{
    mStatusBar->showMessage("Connected");

    setChatEnabled(true);
}

/**
 * @brief Client socket got disconnected
 */
void MainWindow::socketDisconnected()
{
    mStatusBar->showMessage("Disconnected");

    setChatEnabled(false);
}

/**
 * @brief Reacts to socket::hostFound signal
 */
void MainWindow::socketHostFound()
{
    mStatusBar->showMessage("Host found");
}

/**
 * @brief Data are available for reading on the socket
 */
void MainWindow::socketReadyRead()
{
    qDebug() << "ReadyRead";

    QDataStream in(mTcpSocket);
    in.setVersion(QDataStream::Qt_5_2);

    static quint16 blockSize=0;
    QString msg;

    qDebug() << "BlockSize: " << blockSize;
    qDebug() << "bytesAvailable: " << mTcpSocket->bytesAvailable();

    if(blockSize==0)
    {
        if(mTcpSocket->bytesAvailable() < sizeof(quint16))
            return;

        in >> blockSize;
        qDebug() << "Got BlockSize: " << blockSize;
    }

    if(mTcpSocket->bytesAvailable() < blockSize)
        return;

    in >> msg;
    qDebug() << "Got Message: " << msg;

    blockSize = 0;

    ui.textEdit->append(msg);
}


