#include "FileManager.hpp"
#include <QFileDialog>
#include <iostream>
#include <QString>
#include <QFile>
#include <QtDebug>
#include <QMessageBox>

#include <fstream>
#include <sstream>

// Receiving the signal meaning that TextEditor is creating a new empty file.
// we store it in the unknown_file member
void FileManager::newEmptyFile() {
    this->unknown_file = true;
    std::string untitled("untitled");

    std::string fileContent = "";
    emit openedFile(fileContent);    
    
    emit signalFileSavedOrOpened(untitled);

}

void FileManager::openFile() {
     QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", "", "All Files (*)");
     if (!fileName.isEmpty()) {
        std::string name = fileName.toStdString();

        std::ifstream fileToRead(name);
        if (!fileToRead) {
            QMessageBox::warning(nullptr, "Error", "Could not open the selected file.");
            return;
        }

        std::ostringstream buffer;
        buffer << fileToRead.rdbuf(); // read from file to buffer
    
        std::string content = buffer.str();

        this->fileName = name;
        this->unknown_file = false;
        emit openedFile(content);
        emit signalFileSavedOrOpened(name);
    }
        
}


void FileManager::saveFile(std::string content) {
    std::ofstream * file = nullptr;
    if(this->unknown_file) { // if its a new file created by the editor, we need to prompt for a path where to save it
        // Prompt for a path
        std::string newFileName = QFileDialog::getSaveFileName(nullptr, "Save as", "", "All Files (*)").toStdString();

        file = new std::ofstream(newFileName);
        this->fileName = newFileName;
    }
    else {        
        file = new std::ofstream(this->fileName);
    }

    *file << content;

    file->close();
    delete file;


}   