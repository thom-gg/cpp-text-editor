#include "TextBuffer.hpp"
#include <iostream>
#include <cstring>

// Init a TextBuffer with data 
TextBuffer::TextBuffer(const char * data, int dataSize) {
    // Allocate dataSize + gap size and copy data
    // initially the gap is at the end 
    int totalSize = dataSize + GAP_SIZE;
    buffer = new char[totalSize];

    if (buffer == NULL) {
        std::cerr << "Error allocating data for text buffer " << std::endl;
        return;
    }

    bufSize = totalSize;


    // Copy the initial data into the buffer
    memcpy(buffer, data, dataSize);

    // Initialize the left and right pointers
    leftPointer = dataSize;
    rightPointer = totalSize; 
}   


TextBuffer::~TextBuffer() {
    if (buffer != NULL) {
        delete [] buffer;
    }
}


void TextBuffer::insert(char c) {
    // grow if necessary
    if (leftPointer == rightPointer) {
        grow();
    }
    buffer[leftPointer] = c;
    leftPointer += 1;

}


void TextBuffer::grow() {
    // allocate a new one, memcpy and then free the old buf
    int newSize = bufSize + GAP_SIZE;

    char* newBuf = new char[newSize];

    // copy data in the newBuf
    for (int i = 0; i<leftPointer; i++) {
        newBuf[i] = buffer[i];
    }
   
    // then the right part
    for (int i = rightPointer; i<bufSize;i++) {
        newBuf[i + GAP_SIZE] = buffer[i];
    }
    
    // delete old buffer
    delete [] buffer;   
    rightPointer += GAP_SIZE;
    buffer = newBuf;
    bufSize = newSize;

}


void TextBuffer::moveCursor(int position) {
    // need to move the gap left or right
    int currentPos = leftPointer;
    if (position == currentPos) {return;}
    if (position < currentPos) {
        // need to move left
        left(currentPos-position);
    }
    else {
        // need to move right
        right(position-currentPos);

    }
}


void TextBuffer::left(int distance) {
    for (int i = 0; i<distance; i++) {
        leftPointer -= 1;
        rightPointer -=1;
        buffer[rightPointer] = buffer[leftPointer];
    }
}

void TextBuffer::right(int distance) {
    for (int i = 0; i<distance; i++) {
        buffer[leftPointer] = buffer[rightPointer];
        leftPointer +=1;
        rightPointer += 1;
    }
}

char TextBuffer::backspace() {
    // move left pointer one to the left
    if (leftPointer == 0) {return NULL;}
    leftPointer -= 1;
    return buffer[leftPointer];
}

char TextBuffer::delete_after() {
    rightPointer += 1;
    return buffer[rightPointer-1];
}


void TextBuffer::printBuffer() {
    std::cout<<"TextBuffer: ";
    int l = this->length();
    for (int i = 0; i<l; i++) {
        std::cout << this->charAt(i);
    }
    // for (int i = 0; i<leftPointer; i++) {
    //     std::cout<<buffer[i];
    // }
    // for (int i = rightPointer; i<bufSize; i++) {
    //     std::cout<<buffer[i];
    // }
    
    std::cout<<std::endl;
}

int TextBuffer::length() {
    int gapBufferSize = rightPointer - leftPointer;
    return bufSize - gapBufferSize;
}

char TextBuffer::charAt(int index) {
    int gapBufferSize = rightPointer - leftPointer;
    if (index >= (bufSize - gapBufferSize)) {
        // out of bounds
        return NULL;
    }
    
    // adapt index if needed
    if (index >=leftPointer) {
        index += gapBufferSize;
    }

    return buffer[index];
    
}   

char TextBuffer::operator[](int index) {
    return this->charAt(index);
}

void TextBuffer::deleteSelection(int start, int end) {
    // move cursor to start
    this->moveCursor(start);
    for (int i = start; i<end; i++) {
        this->delete_after();
    }
}

