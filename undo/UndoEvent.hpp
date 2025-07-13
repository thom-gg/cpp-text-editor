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
        char c;
        std::string content;
        
        EventType type;
        UndoEvent(int cursorIndex, char c, EventType type, std::string content);
};