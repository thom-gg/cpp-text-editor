#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QTextEdit>
#include "CustomMenu.hpp"
#include "TextEditor.hpp"

class MainWindow : public QMainWindow {
    CustomMenu *customMenu;
    FileManager * fileManager;

    TextEditor * textEditor;

public:
    MainWindow();
    ~MainWindow();
};

#endif // WINDOW_H