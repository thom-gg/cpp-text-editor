#include "FileManager.hpp"
#include <QFileDialog>
#include <iostream>
#include <QString>
#include <QFile>
#include <QtDebug>
#include <QMessageBox>

// Receiving the signal meaning that TextEditor is creating a new empty file.
// we store it in the unknown_file member
void FileManager::newEmptyFile() {
    this->unknown_file = true;
    QString untitled("untitled");
    emit signalFileSavedOrOpened(untitled);
}

void FileManager::openFile() {
     QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", "", "All Files (*)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    QString fileContent = in.readAll();
                    file.close();
                    this->fileName = fileName;
                    this->unknown_file = false;
                    // Emit the signal, with a reference to the content
                    emit openedFile(fileContent);
                    emit signalFileSavedOrOpened(fileName);
            } else {
                // Show an error message if the file couldn't be opened
                QMessageBox::warning(nullptr, "Error", "Could not open the selected file.");
             }

        }
        else {
            // Cancelled the opening
            // do nothing ?
        }
}


void FileManager::saveFile(QString content) {

    QFile * file = nullptr;
    if(this->unknown_file) { // if its a new file created by the editor, we need to prompt for a path where to save it
        // Prompt for a path
        QString newFileName = QFileDialog::getSaveFileName(nullptr, "Save as", "", "All Files (*)");

        file = new QFile(newFileName);
        this->fileName = newFileName;
    }
    else {        
        file = new QFile(this->fileName);
    }

    if (file->open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(file);
            out << content;  
            file->close();    
            this->unknown_file = false; // if we successfully wrote here it is not an unknown file anymore
            emit signalFileSavedOrOpened(this->fileName);
            
    } else {
            // Show an error message if the file couldn't be opened
            QMessageBox::warning(nullptr, "Error", "Could not write to the file: " + fileName);
    }

    delete file;


}   