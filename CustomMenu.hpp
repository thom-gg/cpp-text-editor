#pragma once

#include <QMenuBar>
#include "FileManager.hpp"


class CustomMenu: public QMenuBar {
    Q_OBJECT
    public:
    CustomMenu(QWidget* parent);

    signals:
        // File signals
        void newFileRequested();
        void openFileRequested();
        void openFolderRequested();
        void saveFileRequested();
        void closeFileRequested();
        void exitRequested();
};