/**
 * This file is part of the _SimpleMessenger_ project
 *
 * @author Alexandre Bintz <alexandre.bintz@gmail.com>
 * @date   June 2014
 *
 * @file   mainwindow.h
 * @brief  Program's main window header file
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include "ui_mainwindow.h"
#include "ui_about.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public slots:
    // socket management
    void newConnection();
    void socketHostFound();
    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();
    // errors
    void serverAcceptError(QAbstractSocket::SocketError);
    void socketError(QAbstractSocket::SocketError);
    // chat logic
    void appendLineInChatBox(QString line,bool italic,bool bold,QColor,bool doubleSpacing=true);
    void appendLineInChatBox(QString line,bool italic,bool bold,bool doubleSpacing=true);
    void appendChatMessageFromPeer(QString message);
    void appendChatMessageFromLocal(QString message);
    // user actions
    void clientConnect();
    void serverStart();
    void messageSend();
    // user information
    void showError(QString);
    void showInfo(QString);
    // gui
    void clientServerModeChanged();
    void about();

private:
    //core
    void initCore();
    // gui
    void initUI();
    void setChatEnabled(bool);

private:
    Ui::MainWindow ui;
    Ui::About ui_about;

    QDialog* mAboutDialog;
    QStatusBar* mStatusBar;

    QTcpServer* mTcpServer; // tcp server for server mode
    QTcpSocket* mTcpSocket; // tcp socket for client/server mode

    bool mLastChatBoxLineIsChatMessage = false;
    QString mDateFormat;
};

#endif // MAINWINDOW_H
