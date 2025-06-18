#include "TextEditor.hpp"

// #######
// Undo stack management
// #######



void TextEditor::registerAction(int cursorIndex, char c) {
    this->stack->push(new UndoEvent(cursorIndex, c));
}

void TextEditor::undo() {
    UndoEvent * pop = this->stack->pop();
    int delta = pop->cursorIndex - this->cursorIndex;
    this->moveCursorIndex(delta);
    this->textBuffer->moveCursor(pop->cursorIndex);
    this->textBuffer->delete_after();
}

