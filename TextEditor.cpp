#include "TextEditor.hpp"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QString>
#include <QDebug>

void TextEditor::fileHasBeenOpened(QString & content) {
    // Change the content of the text editor
    this->textEdit->setPlainText(content);
}

void TextEditor::saveFileTriggered() {
    emit saveFileWithContent(textEdit->toPlainText());
}

TextEditor::TextEditor() {
    this->textEdit = new QTextEdit(this);

    // Create a layout and set it to fill the entire widget
     QVBoxLayout *layout = new QVBoxLayout(this);
        
    // Remove any margins to make it fill the entire space
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
        
    // Add the text edit to the layout
    layout->addWidget(textEdit);
        
    // Set the layout for this widget
    setLayout(layout);

}