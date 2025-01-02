#include "TextEditor.hpp"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QString>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>

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

    
}

void TextEditor::saveFileTriggered() {
    emit saveFileWithContent(textEdit->toPlainText());
}


void TextEditor::setupWelcomeScreen(QWidget * textEditor) {
        QVBoxLayout * mainLayout = new QVBoxLayout();
        
        QPushButton* button1 = new QPushButton("New file", textEditor);
        QPushButton* button2 = new QPushButton("Open file", textEditor);
        QPushButton* button3 = new QPushButton("Open folder", textEditor);

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
        
 
}