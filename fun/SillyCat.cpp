#include "SillyCat.hpp"

#include <QWidget>
#include <QPropertyAnimation>
#include <QLabel>
#include <QSequentialAnimationGroup>
#include <iostream>



SillyCat::SillyCat(const QPoint& start, const QPoint& end, int duration, const QSize& desiredSize = QSize(64, 64), QWidget *parent = nullptr) : QWidget(parent), 
                                                                                                                                                image1(SillyCat::path1),
                                                                                                                                                image2(SillyCat::path2),
                                                                                                                                                images{image1,image2} {
                                                                                                                                                    
           
        animationSequence = new QSequentialAnimationGroup(this);
        animationSequence->clear();

     // Create the label that will hold the image
        imageLabel = new QLabel(this);

        // Set widget properties to ensure visibility
        setAttribute(Qt::WA_TranslucentBackground);

       QPixmap scaledImage = image1.scaled(desiredSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // Set the scaled image
        imageLabel->setPixmap(scaledImage);
        
        // Ensure widget size matches image size
        resize(scaledImage.size());
    
        
        
       
        
        // Show everything and start animation
        imageLabel->show();
        show();
        raise();  // Ensure widget is on top


        // 1. First movement animation (to target)
        QPropertyAnimation* moveToTarget = new QPropertyAnimation(this, "pos");
        moveToTarget->setStartValue(start);
        moveToTarget->setEndValue(end);
        moveToTarget->setDuration(duration);
        moveToTarget->setEasingCurve(QEasingCurve::Linear);
        
        // on peut utiliser un qtimer pour alterner deux images et donner un style de "marche"
        alternateTimer = new QTimer(this);
        connect(alternateTimer, &QTimer::timeout, this, [this, desiredSize]() {
            this->cpt = (this->cpt + 1) % 2;
            QPixmap scaledImage = this->images[cpt].scaled(desiredSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            this->imageLabel->setPixmap(scaledImage);
        });
        alternateTimer->start(100);


        connect(moveToTarget, &QPropertyAnimation::finished, this, []() {
                std::cout << "Stealing a word" << std::endl;
        });
        
        // 3. Return movement animation
        QPropertyAnimation* moveBack = new QPropertyAnimation(this, "pos");
        moveBack->setStartValue(end);
        moveBack->setEndValue(start);
        moveBack->setDuration(duration);
        moveBack->setEasingCurve(QEasingCurve::Linear);

         animationSequence->addAnimation(moveToTarget);
        animationSequence->addAnimation(moveBack);
        
         // Start the sequence
        animationSequence->start();
        
        // Connect sequence finished signal
        connect(animationSequence, &QSequentialAnimationGroup::finished, this, [this]() {std::cout<<"finito"<<std::endl;deleteLater();});
        
    }

    // make destructor and free timer inside (maybe free other things)