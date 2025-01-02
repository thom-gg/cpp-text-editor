#include "CustomMenu.hpp"
#include <QMenuBar>
#include <iostream>
#include <QMessageBox>


CustomMenu::CustomMenu(QWidget* parent):QMenuBar(parent) {
    QMenu *fileMenu = addMenu("File");
    
    // add actions to this file menu
    QAction *newAction = new QAction("New file", parent);
    connect(newAction, &QAction::triggered, this, &CustomMenu::newFileRequested);
    fileMenu->addAction(newAction);

    QAction *openFileAct = new QAction("Open file", parent);
    connect(openFileAct, &QAction::triggered, this, &CustomMenu::openFileRequested);
    fileMenu->addAction(openFileAct);

    QAction *openFolderAct = new QAction("Open folder", parent);
    connect(openFolderAct, &QAction::triggered, this, &CustomMenu::openFolderRequested);
    fileMenu->addAction(openFolderAct);

    fileMenu->addSeparator();

    QAction * saveAct = new QAction("Save", parent);
    connect(saveAct, &QAction::triggered, this, &CustomMenu::saveFileRequested);
    fileMenu->addAction(saveAct);

    fileMenu->addSeparator();

    QAction * closeAct = new QAction("Close file", parent);
    connect(closeAct, &QAction::triggered, this, &CustomMenu::closeFileRequested);
    fileMenu->addAction(closeAct);

    fileMenu->addSeparator();

    QAction * exitAct = new QAction("Exit", parent);
    connect(exitAct, &QAction::triggered, this, &CustomMenu::exitRequested);
    fileMenu->addAction(exitAct);


    QMenu *settingsMenu = addMenu("Settings");

    QMenu * aboutMenu = addMenu("About");



}