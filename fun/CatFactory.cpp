#include "CatFactory.hpp"
#include <iostream>

#include <cstdlib> 
#include <ctime>  
#include <QPoint>
#include <QSize>
#include "SillyCat.hpp" 
#include "TextEditor.hpp"

CatFactory::CatFactory(TextEditor * textEditor): textEditor(textEditor) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CatFactory::timerRoutine);


}

void CatFactory::timerRoutine() {
    // deciding if spawn or not
    // random number
    srand(time(0));
    int rdm = rand() % 11;
    if (rdm %2 == 0) {
        spawnCat();
    }
    else {
        std::cout<<"not spawning cat"<<std::endl;
    }

}

void CatFactory::spawnCat() {
    // Spawn a cat
    std::cout<<"spawning a cat" <<std::endl;
    QRect rectangleEditor = textEditor->rect();
    int width = rectangleEditor.width();
    int height = rectangleEditor.height();
    // get random x coord
    srand(time(0));
    int rdmX = rand() % width;
    QPoint start(rdmX, height);
    QPoint end(rectangleEditor.center());  // Account for widget width

    SillyCat * cat = new SillyCat(start, end, 3000, QSize(64, 64), static_cast<QWidget*>(this->textEditor));
}

void CatFactory::startTimer() {
    std::cout<<"starting timer"<<std::endl;
    timer->start(interval * 1000);
}