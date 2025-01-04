#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <QLabel>
#include <QSequentialAnimationGroup>
#include <iostream>
#include <QTimer>

class SillyCat : public QWidget {
    Q_OBJECT


public:
    SillyCat(const QPoint& start, const QPoint& end, int duration, const QSize& desiredSize, QWidget *parent);

private:
    static constexpr const char* path1 = "../img/cat1.png";
    static constexpr const char* path2 = "../img/cat2.png";
    
    int cpt = -1; // cpt used to change image
    QPixmap image1;
    QPixmap image2;
    QPixmap images[2];

    QLabel* imageLabel;
    QTimer * alternateTimer;
    QSequentialAnimationGroup* animationSequence;
};
