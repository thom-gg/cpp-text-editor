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
        void openedFile(std::string &);
        void signalFileSavedOrOpened(std::string fileName);

    public slots:
        void openFile();
        void saveFile(std::string content);
        void newEmptyFile();

    private:
        bool unknown_file = true; // true if its not a real file we're modifying but a new file, to be saved later
        std::string fileName;

};