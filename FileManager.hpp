#pragma once

#include <QFileDialog>
#include <QWidget>
#include <QString>

// Handles file operations (open / save / new)
// optional: file watching for external changes
class FileManager : public QWidget {
    Q_OBJECT
    public:

    signals:
        void openedFile(QString &);


    public slots:
        void openFile();
        void saveFile(QString content);

    private:
        // QString fileContent;
        QString fileName;

};