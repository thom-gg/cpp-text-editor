#include "TextEditor.hpp"

// #########
// Contains all the cursor / selection (and clipboard) logic
// Most of the code is about translating an index in the text to a 2d position on the screen, and vice-versa
// #########

// can be used to find both cursor position and cursorEnd position
int * TextEditor::findCursorPosition(int index) {
    int cursorLine = -1;
    int cursorChar = 0;
    int totalChars = 0;
     for (int i = 0; i<lines.size(); i++) {
         std::string & l = lines[i];
         int lSize = l.size();

        if ( (totalChars + lSize) >= index) {
            cursorLine = i;
            cursorChar = index - totalChars;
            break;
        }
        totalChars += lSize;

        char c = textBuffer->charAt(totalChars);
        if (c == '\n') {totalChars+=1;}
     }
     if (cursorLine == -1) {
        cursorLine = lines.size();
     }
    int cursorX = X_OFFSET + (cursorChar * charWidth);
    int cursorY = Y_OFFSET + (cursorLine * charHeight) - verticalScrollOffset;
    int * res = new int[2]; // up to the caller to free this variable
    res[0] = cursorX;
    res[1] = cursorY;
    return res;
}



int TextEditor::findCursorIndexForPos(int x, int y) {
    int clickedLine = (y-Y_OFFSET) / charHeight;
    if (clickedLine < 0) {
        return 0;
        
    }
    if (clickedLine >=lines.size()) {
        return textBuffer->length();
    }

    const std::string &line = lines[clickedLine];
    int clickedChar = (x-X_OFFSET)/charWidth;
    if (clickedChar < 0) {clickedChar = 0;}
    else if (clickedChar >= line.size()) {
        clickedChar = line.size();
    }

    int cpt = 0;
    for (int i = 0; i < clickedLine; i++) {
        int size = lines[i].size();
        cpt += size;
        char c = textBuffer->charAt(cpt);
        if (c == '\n') {cpt+=1;}
    }
    cpt += clickedChar;
    return cpt;
}



void TextEditor::moveCursorIndex(int delta) {
    // get out of text selection
    syncVerticalOffset = true;

    if (cursorIndex != cursorEndIndex) {
        if (delta < 0) {
            cursorIndex = std::min(cursorIndex, cursorEndIndex);
            cursorEndIndex = cursorIndex;
        }
        else {
            cursorIndex = std::max(cursorIndex, cursorEndIndex);
            cursorEndIndex = cursorIndex;
        }
        update();
        return;
    }
    // regular behavior
    cursorIndex += delta;
    if (cursorIndex < 0) {cursorIndex = 0;}
    int l = this->textBuffer->length();
    if (cursorIndex > l) {cursorIndex = l;}
    cursorEndIndex = cursorIndex;
    update();
}

void TextEditor::moveOneLineUp(bool movingSelection) { // default value movingSelection = false
    syncVerticalOffset = true;

    int index = movingSelection ? cursorEndIndex : cursorIndex;
    int totalChars = 0;
    for (int i = 0; i<lines.size()-1; i++) {        
        std::string & l = lines[i];

        int sizeLine = l.size();        
        char c = textBuffer->charAt(totalChars+sizeLine);
        if (c == '\n') {sizeLine+=1;}
        totalChars += sizeLine;

        if (i == 0 && totalChars > index) {
            // if we were on first line, get to the beginning of it
            index = 0;
            break;
        }

        int totalAtNextLine = totalChars + lines[i+1].size();
        if ( totalAtNextLine >= index) { // if i+1 has the cursor, then i is the new line
            // i is the new line
            int cursorChar = (index - totalChars);
            if (cursorChar > sizeLine) {
                cursorChar = l.size();
            }
            index = (totalChars - sizeLine) + cursorChar;
            
            break;
        }    
    }
    if (!movingSelection) {
        cursorIndex = index;
        cursorEndIndex = cursorIndex;
    }
    else {
        cursorEndIndex = index;
    }
    update();
}

void TextEditor::moveOneLineDown(bool movingSelection) {// default value movingSelection = false
    syncVerticalOffset = true;

    int index = movingSelection ? cursorEndIndex : cursorIndex;
    int totalChars = 0;
    for (int i = 0; i<lines.size(); i++) {
         std::string & l = lines[i];
         int sizeLine = l.size();        
         char c = textBuffer->charAt(totalChars+sizeLine);
         if (c == '\n') {sizeLine+=1;}

        if ( (totalChars + l.size()) >= index) {
            // cursor is in line i
            if (i == (lines.size() - 1)) { // if its last line then move cursor to the very end
                index = this->textBuffer->length();
                break;
            }

            int cursorChar = index - totalChars;
            if (cursorChar > lines[i+1].size()) {
                cursorChar = lines[i+1].size();
            }
            index = totalChars + sizeLine + cursorChar;
            break;
        }
        totalChars += sizeLine;
     }
    if (!movingSelection) {
        cursorIndex = index;
        cursorEndIndex = cursorIndex;
    }
    else {
        cursorEndIndex = index;
    }
    update();

}


void TextEditor::copySelection() {
    if (cursorIndex == cursorEndIndex) {
        return;
    }
    int start = cursorIndex < cursorEndIndex ? cursorIndex : cursorEndIndex;
    int end = cursorIndex < cursorEndIndex ? cursorEndIndex : cursorIndex;
    int size = end-start;
    char * selectedText = new char[size+1];
    selectedText[size] = '\0';
    for (int i = 0; i<size; i++) {
        selectedText[i] = this->textBuffer->charAt(start+i);
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(selectedText);

    delete [] selectedText;

}

void TextEditor::deleteSelection() {
    // remove selection, put cursor on the left then paste
    int start = cursorIndex < cursorEndIndex ? cursorIndex : cursorEndIndex;
    int end = cursorIndex < cursorEndIndex ? cursorEndIndex : cursorIndex;
    this->textBuffer->deleteSelection(start, end);
    cursorIndex = start;
    cursorEndIndex = start;
}

void TextEditor::paste() {
    syncVerticalOffset = true;

    if (cursorIndex != cursorEndIndex) {
        this->deleteSelection();
    }

    // Insert content of clipboard
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString text =  clipboard->text();
    for (int i = 0; i<text.length(); i++) {
        char c = text.at(i).toLatin1();
        this->textBuffer->insert(c);
        cursorIndex += 1;
    }

    cursorEndIndex = cursorIndex;
    
    emit signalFileHasBeenModified();
    update();
}

void TextEditor::selectAll() {
    cursorEndIndex = 0;
    cursorIndex = this->textBuffer->length();
    update();
}