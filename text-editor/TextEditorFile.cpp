#include "TextEditor.hpp"

// #######
// Signals received when an action related to files occured
// #######

void TextEditor::fileHasBeenOpened(std::string &content)
{
    if (textBuffer != nullptr) {
        delete textBuffer;
    }
      
    textBuffer = new TextBuffer(content.data(), content.size());
    cursorIndex = content.size();
    cursorEndIndex = cursorIndex;
    
}

void TextEditor::newEmptyFile()
{
    // We emit a signal to be caught by FileManager to store the info that its not a real file we're working in currently
    emit newEmptyFileRequested();
    // Open an editor with empty content
    std::string emptyContent;
    this->fileHasBeenOpened(emptyContent);
}

// When the save button is pressed, or CTRL + S is pressed
void TextEditor::saveFileTriggered()
{
    // convert to stdstring
    std::string stdString;
    int length = textBuffer->length();
    for (int i = 0; i<length; i++) {
        stdString += textBuffer->charAt(i);
    }
    // emit signal to FileManager
    emit saveFileWithContent(stdString);
    

    QColor originalColor = palette().color(QPalette::Base);
    // Little animation (light green blink), to visually notify that it has been saved
    setStyleSheet("#TextEditor { background-color:rgb(47, 54, 47) }");

    // Create a single-shot timer to revert back to white after 200ms
    QTimer::singleShot(100, this, [this, originalColor]() {
         setStyleSheet(QString("#TextEditor { background-color: %1 }").arg(originalColor.name())); 
    });
}