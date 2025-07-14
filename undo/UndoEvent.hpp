#pragma once
#include <string>

enum EventType {
    INSERTION,
    DELETION
};

class UndoEvent {
    private:
    public:
        int cursorIndex; // index of the beginning of the content
        int lastIndex; // index of the end of the content
        std::string content;
        
        EventType type;
        UndoEvent(int cursorIndex, EventType type, std::string content);
};