#include "FileManager.hpp"
#include <QFileDialog>
#include <iostream>
#include <QString>
#include <QFile>
#include <QtDebug>
#include <QMessageBox>


void FileManager::openFile() {
    std::cout << "In fileManager::openFile()" << std::endl;

         QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File", "", "All Files (*)");
        //  std::cout << fileName << std::endl;
        qDebug().nospace() << "fileName"  << qPrintable(fileName);

        if (!fileName.isEmpty()) {
            QFile file(fileName);

             if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    QString fileContent = in.readAll();
                    file.close();
                    this->fileName = fileName;
                    // Emit the signal, with a reference to the content
                    emit openedFile(fileContent);

                    
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
    std::cout << "In filemanager::saveFile, content = " << std::endl;
    qDebug().nospace() << qPrintable(content);

    QFile file(this->fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;  
        file.close();    
    } else {
        // Show an error message if the file couldn't be opened
        QMessageBox::warning(nullptr, "Error", "Could not write to the file: " + fileName);
    }


}   