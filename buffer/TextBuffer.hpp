#pragma once

// Initial size for the gap
#define GAP_SIZE 10

// Implementing a Gap Buffer to allow quick insertion near cursor position
class TextBuffer {
    public:
        TextBuffer(char * data, int dataSize);
        ~TextBuffer();
        void insert(char c);
        void moveCursor(int position); 

        void printBuffer();

        void backspace();
        void delete_after(); // corresponds to the 'suppr' key on keyboard
    private:
        char * buffer;
        int bufSize;
        // indexes for the gap
        int leftPointer;
        int rightPointer;


        void grow();
        // move the gap (used when cursor is moved)
        void left(int distance);
        void right(int distance);


};