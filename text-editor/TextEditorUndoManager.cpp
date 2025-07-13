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
        this->stack->push(new UndoEvent(cursorIndex, c, DELETION, std::string(1, c)));
    } 
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
        this->stack->push(new UndoEvent(cursorIndex, c, INSERTION, std::string(1, c)));
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

