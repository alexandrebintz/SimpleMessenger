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

/**
 * @brief Constructor
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);

    initUI();
}

/**
 * @brief Shows "About SimpleMessenger" popup.
 */
void MainWindow::about()
{
    mAboutDialog->show();
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
 * @brief Initializes the UI.
 * Should be called only once.
 */
void MainWindow::initUI()
{
    mAboutDialog = new QDialog(this);
    ui_about.setupUi(mAboutDialog);

    connect(ui.actionAboutSimpleMessenger,SIGNAL(triggered()),
            this,SLOT(about()));
    connect(ui.actionAboutQt,SIGNAL(triggered()),
            qApp,SLOT(aboutQt()));

    connect(ui.radioButtonClientMode,SIGNAL(toggled(bool)),
            this,SLOT(clientServerModeChanged()));
    connect(ui.radioButtonClientMode,SIGNAL(toggled(bool)),
            this,SLOT(clientServerModeChanged()));

    ui.radioButtonClientMode->setChecked(false);
}


