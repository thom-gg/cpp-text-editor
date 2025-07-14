#include "TextEditor.hpp"

// #######
// Undo stack management
// #######

void TextEditor::registerDeletion(int cursorIndex, char c) {
    if (c == NULL) {return;}
    UndoEvent * topElement = this->stack->top();
    if (topElement != NULL
        && (topElement->type == DELETION)
        && (topElement->lastIndex == (cursorIndex + 1))) {
            topElement->content.insert(0, 1, c);
            topElement->lastIndex -= 1;
    }   
    else {
        this->stack->push(new UndoEvent(cursorIndex, DELETION, std::string(1, c)));
    } 
}

void TextEditor::registerDeleteSelection(int startIndex, int endIndex) {
    std::string content = std::string(1,this->textBuffer->charAt(startIndex));
    for (int i = startIndex+1; i<endIndex; i++) {
        content.append(1, this->textBuffer->charAt(i));
    }
    UndoEvent * newEvent = new UndoEvent(endIndex-1, DELETION, content);
    newEvent->lastIndex = startIndex+1;
    this->stack->push(newEvent);
}

void TextEditor::registerPaste(int startIndex, QString q_content) {
    std::string content = q_content.toStdString();
    this->stack->push(new UndoEvent(startIndex, INSERTION, content));
}

void TextEditor::registerInsertion(int cursorIndex, char c) {
    // Get top, if top is the same we just stack it
    UndoEvent * topElement = this->stack->top();
    if (topElement != NULL 
        && (topElement->type == INSERTION)
        && (topElement->lastIndex == (cursorIndex-1)) 
        && c != ' ') {
        // append to top element modification
        topElement->content.append(1,c);
        topElement->lastIndex += 1;
    }
    else {
        this->stack->push(new UndoEvent(cursorIndex, INSERTION, std::string(1, c)));
    }
}

void TextEditor::undo() {
    UndoEvent * pop = this->stack->pop();
    if (pop == NULL) {return;}
    if (pop->type == INSERTION) {
        int delta = pop->cursorIndex - this->cursorIndex;
        this->moveCursorIndex(delta);
        this->textBuffer->moveCursor(pop->cursorIndex);
        for (int i = 0; i<pop->content.size(); i++) {
            this->textBuffer->delete_after();
        }
    }
    else if (pop->type == DELETION) {
        // we need to go at last index where we most recently deleted and insert back
        int delta = (pop->lastIndex-1) - this->cursorIndex;
        this->moveCursorIndex(delta);
        this->textBuffer->moveCursor(pop->lastIndex - 1);
        for (int i = 0; i<pop->content.size(); i++) {
            this->textBuffer->insert(pop->content[i]);
        }
        this->moveCursorIndex(pop->content.size());
    }
}

