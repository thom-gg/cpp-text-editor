#include "MainWindow.hpp"
#include <QPushButton>
#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QIcon>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QMessageBox>
#include <iostream>
#include "CustomMenu.hpp"
#include <QApplication>
#include <QScreen>
#include <QRect>

#include "FileManager.hpp"


void MainWindow::textHasChanged() {
    QString currentTitle = this->windowTitle();
    QChar firstChar = currentTitle.at(0);
    // If already a '*', no need to do anything
    if (firstChar == '*') {
        return;
    }
    else {
        currentTitle.insert(0, '*');
        setWindowTitle(currentTitle);
    }

}

void MainWindow::fileWasSavedOrOpened(std::string fileName) {
    setWindowTitle("Text editor: " + QString::fromStdString(fileName));
}

MainWindow::MainWindow() {
        customMenu = new CustomMenu(this);
        this->setMenuBar(customMenu);

        fileManager = new FileManager();


        textEditor = new TextEditor();
        setCentralWidget(textEditor);
        

        // Connect the signal from customMenu to a handler
        connect(customMenu, &CustomMenu::openFileRequested, fileManager, &FileManager::openFile);
        connect(textEditor, &TextEditor::openFileRequested, fileManager, &FileManager::openFile);

        connect(customMenu, &CustomMenu::newFileRequested, textEditor, &TextEditor::newEmptyFile);
        connect(textEditor, &TextEditor::newEmptyFileRequested, fileManager, &FileManager::newEmptyFile);

        connect(customMenu, &CustomMenu::saveFileRequested, textEditor, &TextEditor::saveFileTriggered);
        connect(textEditor, &TextEditor::saveFileWithContent, fileManager, &FileManager::saveFile);
        connect(fileManager, &FileManager::openedFile, textEditor, &TextEditor::fileHasBeenOpened);

        connect(textEditor, &TextEditor::signalFileHasBeenModified, this, &MainWindow::textHasChanged);
        connect(fileManager, &FileManager::signalFileSavedOrOpened, this, &MainWindow::fileWasSavedOrOpened);

        // Set up the main window
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect  screenGeometry = screen->geometry();
        int height = screenGeometry.height();
        int width = screenGeometry.width();
        setWindowTitle("Text editor");
        // make the window 75% of screen size
        resize(75 * width / 100, 75 * height / 100);
    }

MainWindow::~MainWindow() {
    delete customMenu;
    delete fileManager;
    delete textEditor;

}