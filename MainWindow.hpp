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
public slots:
    void textHasChanged(); // catching a signal sent by TextEditor, to update the window title
    void fileWasSavedOrOpened(std::string fileName); // catching a signal sent by FileManager, after file was saved succesfully (to update the title);
};

#endif // WINDOW_H