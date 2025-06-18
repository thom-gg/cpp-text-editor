#pragma once

class UndoEvent {
    private:
    public:
        int cursorIndex;
        char c;
        UndoEvent(int cursorIndex, char c);
};