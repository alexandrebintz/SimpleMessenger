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
#include "ui_mainwindow.h"
#include "ui_about.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public slots:
    // core
    void clientServerModeChanged();
    // gui
    void about();

private:
    void initUI();

private:
    Ui::MainWindow ui;
    Ui::About ui_about;

    QDialog* mAboutDialog;
};

#endif // MAINWINDOW_H
