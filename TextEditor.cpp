#include "TextEditor.hpp"
#include <QTextEdit>
#include <iostream>
#include <QVBoxLayout>
#include <QString>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QShortcut>
#include <QTextCursor>

#include <QTimer>

void TextEditor::fileHasBeenOpened(QString & content) {
    if (this->textEdit == nullptr) {
        this->textEdit = new QTextEdit();
        

        QVBoxLayout * mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        mainLayout->addWidget(this->textEdit);
        if (this->layout()) {
            delete this->layout(); // Clean up the old layout
        }
        this->setLayout(mainLayout);
    }

    // Change the content of the text editor
    this->textEdit->setPlainText(content);
    // Set cursor initial position
    QTextCursor cursor = textEdit->textCursor();        
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);    
}

void TextEditor::newEmptyFile() {
    // We emit a signal to be caught by FileManager to store the info that its not a real file we're working in currently
    emit newEmptyFileRequested(); 
    // Open an editor with empty content
    QString emptyContent;
    this->fileHasBeenOpened(emptyContent);


}

// When the save button is pressed, or CTRL + S is pressed
void TextEditor::saveFileTriggered() {
    if (this->textEdit == nullptr) {
        return; // we're on welcome menu
    }
    // Emit signal to be caught by FileManager and actually saves the file. We pass the content (QString) as parameter
    emit saveFileWithContent(textEdit->toPlainText());

    // Little animation (light green blink), to visually notify that it has been saved
     this->textEdit->setStyleSheet("QTextEdit { background-color: #e6ffe6 }");  

     // Create a single-shot timer to revert back to white after 200ms
     QTimer::singleShot(100, this, [this]() {
        textEdit->setStyleSheet("QTextEdit { background-color: white }");
     });
    
}


void TextEditor::setupWelcomeScreen(QWidget * textEditor) {
        QVBoxLayout * mainLayout = new QVBoxLayout();
        
        QPushButton* button1 = new QPushButton("New file", textEditor);
        QPushButton* button2 = new QPushButton("Open file", textEditor);
        QPushButton* button3 = new QPushButton("Open folder", textEditor);


        connect(button1, &QPushButton::clicked, this, &TextEditor::newEmptyFile);
        connect(button2, &QPushButton::clicked, this, [this]() {emit openFileRequested();});


        button1->setMinimumSize(100, 50); // Set minimum size for buttons
        button2->setMinimumSize(100, 50);
        button3->setMinimumSize(100, 50);
       
        QHBoxLayout * row = new QHBoxLayout();
        // Add buttons to the layouts
        row->addSpacing(50);
        row->addWidget(button1);
        row->addSpacing(10);
        row->addWidget(button2);
        row->addSpacing(10);
        row->addWidget(button3);
        row->addStretch();

        mainLayout->addSpacing(50);
        mainLayout->addLayout(row);
        mainLayout->addStretch();
        

        // Apply layout to the widget
        textEditor->setLayout(mainLayout);
}

TextEditor::TextEditor() {
     setupWelcomeScreen(this);

    // Bind CTRL+S to save slot
    QShortcut * saveShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    saveShortcut->setAutoRepeat(false); // so it doesnt spam trigger when we keep it pressed
    connect(saveShortcut, &QShortcut::activated, this, &TextEditor::saveFileTriggered);

        
 
}