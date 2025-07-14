#include "UndoEvent.hpp"




UndoEvent::UndoEvent(int cursorIndex, EventType type, std::string content) {
    this->cursorIndex = cursorIndex;
    this->type = type;
    this->content = content;
    this->lastIndex = cursorIndex;
}