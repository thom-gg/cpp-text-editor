#include "UndoEvent.hpp"




UndoEvent::UndoEvent(int cursorIndex, char c, EventType type, std::string content) {
    this->cursorIndex = cursorIndex;
    this->c = c;
    this->type = type;
    this->content = content;
    this->lastIndex = cursorIndex;
}