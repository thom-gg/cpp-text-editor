#include "TextEditor.hpp"

// #######
// Undo / Redo stack management
// #######

/*
Anytime something is pushed into the UNDO stack, the REDO stack is cleared.
*/
void TextEditor::clearRedo() {
    this->redoStack->flush();
}

void TextEditor::registerDeletion(int cursorIndex, char c) {
    if (c == NULL) {return;}
    clearRedo();
    UndoEvent * topElement = this->undoStack->top();
    if (topElement != NULL
        && (topElement->type == DELETION)
        && (topElement->lastIndex == (cursorIndex + 1))) {
            topElement->content.insert(0, 1, c);
            topElement->lastIndex -= 1;
    }   
    else {
        this->undoStack->push(new UndoEvent(cursorIndex, cursorIndex-1, DELETION, std::string(1, c)));
    } 
}

void TextEditor::registerDeleteSelection(int startIndex, int endIndex) {
    clearRedo();
    std::string content = std::string(1,this->textBuffer->charAt(startIndex));
    for (int i = startIndex+1; i<endIndex; i++) {
        content.append(1, this->textBuffer->charAt(i));
    }
    UndoEvent * newEvent = new UndoEvent(endIndex, startIndex, DELETION, content);
    newEvent->lastIndex = startIndex;
    this->undoStack->push(newEvent);
}

void TextEditor::registerPaste(int startIndex, QString q_content) {
    clearRedo();
    std::string content = q_content.toStdString();
    this->undoStack->push(new UndoEvent(startIndex, startIndex, INSERTION, content));
}

void TextEditor::registerInsertion(int cursorIndex, char c) {
    clearRedo();
    // Get top, if top is the same we just append to it
    UndoEvent * topElement = this->undoStack->top();
    if (topElement != NULL 
        && (topElement->type == INSERTION)
        && (topElement->lastIndex == (cursorIndex-1)) 
        && c != ' ') {
        // append to top element modification
        topElement->content.append(1,c);
        topElement->lastIndex += 1;
    }
    else {
        this->undoStack->push(new UndoEvent(cursorIndex, cursorIndex, INSERTION, std::string(1, c)));
    }
}

void TextEditor::undo() {
    UndoEvent * pop = this->undoStack->pop();
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
        int delta = (pop->lastIndex) - this->cursorIndex;
        this->moveCursorIndex(delta);
        this->textBuffer->moveCursor(pop->lastIndex);
        for (int i = 0; i<pop->content.size(); i++) {
            this->textBuffer->insert(pop->content[i]);
        }
        this->moveCursorIndex(pop->content.size());
    }
    this->redoStack->push(pop);
}



void TextEditor::redo() {
    UndoEvent * pop = this->redoStack->pop();
    if (pop == NULL) {return;}
    if (pop->type == INSERTION) {
        int delta = (pop->cursorIndex) - this->cursorIndex;
        this->moveCursorIndex(delta);
        this->textBuffer->moveCursor(pop->cursorIndex);
        for (int i = 0; i<pop->content.size(); i++) {
            this->textBuffer->insert(pop->content[i]);
        }
        this->moveCursorIndex(pop->content.size());
    }
    else if (pop->type == DELETION) {
        int delta = pop->lastIndex - this->cursorIndex;
        this->moveCursorIndex(delta);
        this->textBuffer->moveCursor(pop->lastIndex);
        for (int i = 0; i<pop->content.size(); i++) {
            this->textBuffer->delete_after();
        }
    }

    this->undoStack->push(pop);
}
