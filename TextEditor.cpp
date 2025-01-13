#include "TextEditor.hpp"
#include <QTextEdit>
#include <iostream>
#include <QVBoxLayout>
#include <QString>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QShortcut>
#include <QTextCursor>
#include <QPainter>
#include <QTimer>
#include <QPoint>
#include <QPixmap>

#include "SillyCat.hpp"
#include "CatFactory.hpp"

void TextEditor::fileHasBeenOpened(QString &content)
{
    if (this->textEdit == nullptr)
    {
        setupQTextEdit();
        factory->startTimer();
    }

    // Change the content of the text editor
    this->textEdit->setPlainText(content);
    // Set cursor initial position
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
}

void TextEditor::newEmptyFile()
{
    // We emit a signal to be caught by FileManager to store the info that its not a real file we're working in currently
    emit newEmptyFileRequested();
    // Open an editor with empty content
    QString emptyContent;
    this->fileHasBeenOpened(emptyContent);
}

// When the save button is pressed, or CTRL + S is pressed
void TextEditor::saveFileTriggered()
{
    if (this->textEdit == nullptr)
    {
        return; // we're on welcome menu
    }
    // Emit signal to be caught by FileManager and actually saves the file. We pass the content (QString) as parameter
    emit saveFileWithContent(textEdit->toPlainText());

    QColor originalColor = textEdit->palette().color(QPalette::Base);
    // Little animation (light green blink), to visually notify that it has been saved
    this->textEdit->setStyleSheet("QTextEdit { background-color:rgb(47, 54, 47) }");

    // Create a single-shot timer to revert back to white after 200ms
    QTimer::singleShot(100, this, [this, originalColor]()
                       { textEdit->setStyleSheet(QString("QTextEdit { background-color: %1 }")
                                                     .arg(originalColor.name())); });
}

void TextEditor::setupWelcomeScreen(QWidget *textEditor)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QPushButton *button1 = new QPushButton("New file", textEditor);
    QPushButton *button2 = new QPushButton("Open file", textEditor);
    QPushButton *button3 = new QPushButton("Open folder", textEditor);

    connect(button1, &QPushButton::clicked, this, &TextEditor::newEmptyFile);
    connect(button2, &QPushButton::clicked, this, [this]()
            { emit openFileRequested(); });

    button1->setMinimumSize(100, 50); // Set minimum size for buttons
    button2->setMinimumSize(100, 50);
    button3->setMinimumSize(100, 50);

    QHBoxLayout *row = new QHBoxLayout();
    // Add buttons to the layouts
    row->addSpacing(50);
    row->addWidget(button1);
    row->addSpacing(10);
    row->addWidget(button2);
    row->addSpacing(10);
    row->addWidget(button3);
    row->addStretch();

    mainLayout->addSpacing(50);
    mainLayout->addLayout(row);
    mainLayout->addStretch();

    // Apply layout to the widget
    textEditor->setLayout(mainLayout);
}

void TextEditor::updateZooming(int amount)
{
    // will zoomOut if amount is negative
    textEdit->zoomIn(amount);
}

// Handle mouse wheel
void TextEditor::wheelEvent(QWheelEvent *event)
{
    const double degrees = event->angleDelta().y() / 8.0;
    updateZooming((degrees / 5));
}

// Called when we get out of welcome screen
void TextEditor::setupQTextEdit()
{
    this->textEdit = new QTextEdit();
    this->textEdit->zoomIn(5);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(this->textEdit);
    if (this->layout())
    {
        delete this->layout(); // Clean up the old layout
    }
    this->setLayout(mainLayout);

    // Connect the "textChanged" signal from QTextEdit class to a custom one we're sending
    // (why? because I plan on getting rid of QTextEdit class later and implement all of this myself)
    connect(this->textEdit, &QTextEdit::textChanged, this, [this]()
            { emit signalFileHasBeenModified(); });
}

void TextEditor::printNewLine(int &xCoord, int &yCoord, int &charHeight, int &lineNumber, QPainter &painter)
{
    yCoord += charHeight;
    lineNumber += 1;
    painter.setPen(Qt::gray);
    painter.drawText(QRect(0, yCoord, charWidth*2, charHeight), 0, QString("%1:").arg(lineNumber));
    painter.setPen(Qt::white);

    xCoord = X_OFFSET;
}

void TextEditor::paintEvent(QPaintEvent *event)
{

    // Drawing text
    QPainter painter(this);
    painter.setPen(Qt::white);
    // using a monospace font so we can count the width for each character and know where to break line
    painter.setFont(QFont("Courier", 20));
    
    

    QRect bounds = painter.boundingRect(rect(), Qt::AlignLeft, "A");
    charWidth = bounds.width();
    charHeight = bounds.height();
    Y_OFFSET = charHeight;
    int maxWidth = width();
    int maxHeight = height();

    // draw a grid
    for (int i = 0; i<maxWidth; i+=charWidth) {
        for (int j = 0; j<maxHeight; j+=charHeight) {
            painter.drawRect(i,j,charWidth, charHeight);
        }
    }

    QString currWord("");

    int lineNumber = 0;
    int xCoord = X_OFFSET;
    int yCoord = 0;

    printNewLine(xCoord, yCoord, charHeight, lineNumber, painter);

    int length = textBuffer->length();

    // We iterate on each char (and once after to print the last word)
    for (int i = 0; i <= length; i++)
    {
       
        char c;
        if (i < length)
        {
            c = textBuffer->charAt(i);

            if (c != '\n')
            { // we dont draw a newline char, we handle it ourselves using our printNewLine
                QChar ch = c;
                currWord += ch;
            }
        }

        if (i == length || c == ' ' || c == '\n')
        { // when we encounter a space or for last iteration
            int wordWidth = currWord.length() * charWidth;
            if ((xCoord + wordWidth) > maxWidth)
            {
                // break line if needed
                xCoord = X_OFFSET;
                yCoord += charHeight;
            }
            // print the word we just completed
            painter.drawText(QRect(xCoord, yCoord, wordWidth, charHeight), Qt::AlignCenter, currWord);

            xCoord += wordWidth;
            currWord.clear();
        }

        if (c == '\n')
        {
            printNewLine(xCoord, yCoord, charHeight, lineNumber, painter);
        }
         if (i == cursorPosition) {
            // draw cursor
            painter.drawLine(xCoord + (currWord.length() * charWidth), 
                            yCoord-charHeight+5, 
                            xCoord + (currWord.length() * charWidth), 
                            yCoord);
        }
    }
    painter.setPen(Qt::red);

    painter.drawLine(X_OFFSET + (cursorX*charWidth), 
                        (Y_OFFSET + (cursorY * charHeight) ),
                        X_OFFSET + (cursorX*charWidth),
                        (Y_OFFSET + (cursorY * charHeight) )+charHeight);

    // draw cursor
    // painter.drawLine(cursorXCoord, cursorYCoord-charHeight+5, cursorXCoord, cursorYCoord);
}

void TextEditor::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {return;}
    // QPointF pos = event->position();
    int x = event->x();
    int y = event->y();
    if (x < X_OFFSET || y < Y_OFFSET) {return;}
    // determine if its left or right of the char
   
    
    int xCoord = (x-X_OFFSET) / charWidth;
    int yCoord = (y-Y_OFFSET) / charHeight; 
    int offset = x % charWidth;
    if (offset >= (charWidth / 2)) {
        xCoord += 1;
        std::cout<<"right side of the char"<<std::endl;
    }
    else {
        std::cout<<"left sidze of the char"<<std::endl;
    }
    cursorX = xCoord;
    cursorY = yCoord;
    update();
    std::cout << "mouse prsees event x:"<<x<<" y:"<<y << " grid = " << xCoord <<","<<yCoord<< std::endl;

}


void TextEditor::moveCursor(int deltaX, int deltaY) {
    this->cursorX += deltaX;
    this->cursorY += deltaY;
    if (this->cursorX < 0) {cursorX = 0;}
    if (this->cursorY < 0) {cursorY = 0;}
    
    update();
}

TextEditor::TextEditor()
{
    // Dummy data to try the paint function
    textBuffer = new TextBuffer("Hello !\nThis is a test text to try printing test on the screen by myself without using the QTextEdit component.", 111);
    cursorPosition = 2;
    //  setupWelcomeScreen(this);



    QShortcut * rightArrow = new QShortcut(QKeySequence::MoveToNextChar, this);
    connect(rightArrow, &QShortcut::activated, this, [this]() {
        this->TextEditor::moveCursor(1, 0);
        });
        
    QShortcut * leftArrow = new QShortcut(QKeySequence::MoveToPreviousChar, this);
    connect(leftArrow, &QShortcut::activated, this, [this]() {
        this->TextEditor::moveCursor(-1, 0);
    });

    QShortcut * upArrow = new QShortcut(QKeySequence::MoveToPreviousLine, this);
    connect(upArrow, &QShortcut::activated, this, [this]() {
        this->TextEditor::moveCursor(0, -1);
    });

    QShortcut * downArrow = new QShortcut(QKeySequence::MoveToNextLine, this);
    connect(downArrow, &QShortcut::activated, this, [this]() {
        this->TextEditor::moveCursor(0, 1);
    });

    // // Bind CTRL+S to save slot
    // QShortcut * saveShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    // saveShortcut->setAutoRepeat(false); // so it doesnt spam trigger when we keep it pressed
    // connect(saveShortcut, &QShortcut::activated, this, &TextEditor::saveFileTriggered);

    // QShortcut * zoomInShortcut = new QShortcut(QKeySequence::ZoomIn, this);
    // connect(zoomInShortcut, &QShortcut::activated, this, [this]() {updateZooming(10);});

    // QShortcut * zoomOutShortcut = new QShortcut(QKeySequence::ZoomOut, this);
    // connect(zoomOutShortcut, &QShortcut::activated, this, [this]() {updateZooming(-10);});

    // factory = new CatFactory(this);
}

TextEditor::~TextEditor()
{
    if (textBuffer != nullptr)
    {
        delete textBuffer;
    }
    if (factory != nullptr)
    {
        delete factory;
    }
    if (textEdit != nullptr)
    {
        delete textEdit;
    }
}

