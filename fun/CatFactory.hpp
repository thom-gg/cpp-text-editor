#pragma once

#include <QTimer>
#include <QWidget>


class TextEditor; // for circular dependencies

// Every Xsec has a 50% chance of spawning a SillyCat
// Only starts once we are in a file (not in welcome screen)
class CatFactory: public QWidget {
    Q_OBJECT
    private:
        static const int interval = 5; // number of seconds        
        QTimer * timer;
        TextEditor * textEditor;
        void timerRoutine();
        void spawnCat();

    public:        
        CatFactory(TextEditor *);
        void startTimer();

    
};