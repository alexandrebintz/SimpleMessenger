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
    // core - low level
    void newConnection();
    void socketHostFound();
    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();
    // core - errors
    void serverAcceptError(QAbstractSocket::SocketError);
    void socketError(QAbstractSocket::SocketError);
    // core - high level
    void clientConnect();
    void serverStart();
    void messageSend();
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
};

#endif // MAINWINDOW_H
