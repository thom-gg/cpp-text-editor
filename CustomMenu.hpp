#pragma once

#include <QMenuBar>
#include "FileManager.hpp"


class CustomMenu: public QMenuBar {
    Q_OBJECT
    public:
    CustomMenu(QWidget* parent);
    ~CustomMenu();

    private:
        QMenu * fileMenu;
        QAction * newAction;
        QAction * openFileAct;
        QAction * openFolderAct;
        QAction * saveAct;
        QAction * closeAct;
        QAction * exitAct;

    signals:
        // File signals
        void newFileRequested();
        void openFileRequested();
        void openFolderRequested();
        void saveFileRequested();
        void closeFileRequested();
        void exitRequested();
};