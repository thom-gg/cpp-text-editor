#include "UndoEvent.hpp"


UndoEvent::UndoEvent(int cursorIndex, char c) {
    this->cursorIndex = cursorIndex;
    this->c = c;
}