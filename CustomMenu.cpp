#include "CustomMenu.hpp"
#include <QMenuBar>
#include <iostream>
#include <QMessageBox>


CustomMenu::CustomMenu(QWidget* parent):QMenuBar(parent) {
    fileMenu = addMenu("File");
    
    // add actions to this file menu
    newAction = new QAction("New file", parent);
    connect(newAction, &QAction::triggered, this, &CustomMenu::newFileRequested);
    fileMenu->addAction(newAction);

    openFileAct = new QAction("Open file", parent);
    connect(openFileAct, &QAction::triggered, this, &CustomMenu::openFileRequested);
    fileMenu->addAction(openFileAct);

    openFolderAct = new QAction("Open folder", parent);
    connect(openFolderAct, &QAction::triggered, this, &CustomMenu::openFolderRequested);
    fileMenu->addAction(openFolderAct);

    fileMenu->addSeparator();

    saveAct = new QAction("Save", parent);
    connect(saveAct, &QAction::triggered, this, &CustomMenu::saveFileRequested);
    fileMenu->addAction(saveAct);

    fileMenu->addSeparator();

    closeAct = new QAction("Close file", parent);
    connect(closeAct, &QAction::triggered, this, &CustomMenu::closeFileRequested);
    fileMenu->addAction(closeAct);

    fileMenu->addSeparator();

    exitAct = new QAction("Exit", parent);
    connect(exitAct, &QAction::triggered, this, &CustomMenu::exitRequested);
    fileMenu->addAction(exitAct);

}

CustomMenu::~CustomMenu() {
    delete fileMenu;
    delete newAction;
    delete openFileAct;
    delete openFolderAct;
    delete saveAct;
    delete closeAct;
    delete exitAct;
}