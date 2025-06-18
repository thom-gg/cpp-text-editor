#pragma once

#include <iostream>

#define INITIAL_SIZE 32

// Class declaration
template <typename U > 
class Stack {
    private:
        U * array = nullptr;
        int physicalSize;   // allocated size
        int logicalSize; // number of elements

    public:
        Stack();
        ~Stack();
        U pop();
        U top();
        void push(U elt);
        void flush();

        void printStack();



};

// Implementation
template <typename U>
Stack<U>::Stack() {
    std::cout << "In Stack constructor" << std::endl;
    std::cout << this->array << std::endl;

    this->array = new U[INITIAL_SIZE];
    

    this->physicalSize = INITIAL_SIZE;
    this->logicalSize = 0;
}

template <typename U>
void Stack<U>::printStack() {
    std::cout<<"Printing stack: physical size = " << this->physicalSize << " logical size = " << this->logicalSize <<std::endl;
    for (int i = 0; i<this->logicalSize; i++) {
        std::cout << (char)this->array[i];
    }
    std::cout << std::endl;
}

template <typename U>
Stack<U>::~Stack() {
    delete [] this->array;
}

template <typename U>
void Stack<U>::flush() {
    this->logicalSize = 0;
}

template <typename U>
void Stack<U>::push(U elt) {
    if (physicalSize == logicalSize) {
        // Needs to expand the array
        // Allocate a new one, copy data inside, and free the old one
        int newSize = physicalSize + INITIAL_SIZE;
        U * newArray = new U[newSize];
        U * oldArray = this->array;
        this->array = newArray;
        // Copy data inside
        for (int i = 0; i<logicalSize; i++) {
            this->array[i] = oldArray[i];
        }
        physicalSize = newSize;

        delete [] oldArray;
    }

    this->array[logicalSize] = elt;
    this->logicalSize += 1;
}

template <typename U>
U Stack<U>::pop() {
    if (logicalSize == 0) {
        throw std::out_of_range("The stack is empty - cannot pop");
    }
    U elt = this->array[logicalSize-1];
    this->logicalSize -= 1;
    
    return elt;
}

template <typename U>
U Stack<U>::top() {
    if (logicalSize == 0) {
        throw std::out_of_range("The stack is empty - cannot pop");
    }    
    return this->array[logicalSize-1];
}