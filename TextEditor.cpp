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
#include <QStyleOption>

#include "SillyCat.hpp"
#include "CatFactory.hpp"

void TextEditor::fileHasBeenOpened(QString &content)
{
    delete textBuffer;
    std::string stdString = content.toStdString();
    

    textBuffer = new TextBuffer(stdString.data(), stdString.size());
    cursorIndex = stdString.size();
    
    // if (this->textEdit == nullptr)
    // {
    //     setupQTextEdit();
    //     factory->startTimer();
    // }

    // // Change the content of the text editor
    // this->textEdit->setPlainText(content);
    // // Set cursor initial position
    // QTextCursor cursor = textEdit->textCursor();
    // cursor.movePosition(QTextCursor::End);
    // textEdit->setTextCursor(cursor);
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
    // convert to stdstring
    std::string stdString;
    int length = textBuffer->length();
    for (int i = 0; i<length; i++) {
        stdString += textBuffer->charAt(i);
    }
    // emit signal to FileManager
    emit saveFileWithContent(QString::fromStdString(stdString));
    

    QColor originalColor = palette().color(QPalette::Base);
    // Little animation (light green blink), to visually notify that it has been saved
    setStyleSheet("#TextEditor { background-color:rgb(47, 54, 47) }");

    // Create a single-shot timer to revert back to white after 200ms
    QTimer::singleShot(100, this, [this, originalColor]() {
         setStyleSheet(QString("#TextEditor { background-color: %1 }").arg(originalColor.name())); 
    });
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
    // textEdit->zoomIn(amount);
}

// Handle mouse wheel
// void TextEditor::wheelEvent(QWheelEvent *event)
// {
//     const double degrees = event->angleDelta().y() / 8.0;
//     updateZooming((degrees / 5));
// }

// Called when we get out of welcome screen
void TextEditor::setupQTextEdit()
{
    // this->textEdit = new QTextEdit();
    // this->textEdit->zoomIn(5);

    // QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // mainLayout->setContentsMargins(0, 0, 0, 0);
    // mainLayout->setSpacing(0);

    // mainLayout->addWidget(this->textEdit);
    // if (this->layout())
    // {
    //     delete this->layout(); // Clean up the old layout
    // }
    // this->setLayout(mainLayout);

    // // Connect the "textChanged" signal from QTextEdit class to a custom one we're sending
    // // (why? because I plan on getting rid of QTextEdit class later and implement all of this myself)
    // connect(this->textEdit, &QTextEdit::textChanged, this, [this]()
    //         { emit signalFileHasBeenModified(); });
}

void TextEditor::printNewLine(int lineIndex, int & linesNumber, QPainter &painter)
{
    linesNumber += 1; // draw line number
    painter.setPen(Qt::gray);
    painter.setFont(QFont("Courier", 15));
    painter.drawText(QRect(0, (Y_OFFSET + (lineIndex) * charHeight) - verticalScrollOffset, charWidth*2, charHeight), Qt::AlignLeft | Qt::AlignVCenter, QString("%1:").arg(linesNumber));
    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier", 20));

}

void TextEditor::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);  
    QPainter painter(this);
    // drawing the style sheet before doing our custom painting event (used for background color painting (save file blink for example))
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    painter.setPen(Qt::white);
    // using a monospace font so we can count the width for each character and know where to break line
    painter.setFont(QFont("Courier", 20));
    
    lines.clear();


    QRect bounds = painter.boundingRect(rect(), Qt::AlignLeft, "A");
    charWidth = bounds.width();
    charHeight = bounds.height();
    Y_OFFSET = charHeight;
    int maxWidth = width();
    int maxHeight = height();     
    // // draw a grid   
    // painter.setOpacity(0.1);
    // for (int i = 0; i<maxWidth; i+=charWidth) {
    //     for (int j = 0; j<maxHeight; j+=charHeight) {
    //         painter.drawRect(i,j,charWidth, charHeight);
    //     }
    // }
    // painter.setOpacity(1);

    // split text into lines, handline \n and line wrapping
    int linesNumber = 0;
    std::string currentLine;
    std::string currentWord;
    int currentWidth = X_OFFSET;

    int length = textBuffer->length();
    printNewLine(lines.size(), linesNumber, painter);

    for (int i = 0; i < length; i++) {
        char c = textBuffer->charAt(i);

        if (c == ' ' || c == '\n') {
            // end of current  word
            if ((currentWidth + (charWidth* currentWord.size())) > maxWidth) {
                // word doesnt fit
                lines.push_back(currentLine);
                currentLine.clear();
                currentWidth = X_OFFSET;
            }

            if (c == ' ') {
                currentWord += c;
                currentWidth += 1;
            }
            currentLine += currentWord;
            currentWidth += charWidth * currentWord.size();
            currentWord.clear();

            if (c == '\n') {
                lines.push_back(currentLine);
                currentLine.clear();
                currentWidth = X_OFFSET;

                
                printNewLine( lines.size(), linesNumber, painter);

                
            }
        } else {
            currentWord += c;
        }
    }

    // if last word doesnt fit, make it go to next line
    if (!currentWord.empty()) {
        if ((currentWidth + (charWidth * currentWord.size())) > maxWidth) {
        // Word doesn't fit, push the current line
        lines.push_back(currentLine);
        currentLine = currentWord;
        } else {
        currentLine += currentWord;
        }
    }

    // add last line
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

   
    // draw text line by line
    int yCoord = Y_OFFSET - verticalScrollOffset;
    for (int i = 0; i<lines.size(); i++) {
        std::string & l = lines[i];
        for (int j = 0; j < l.size(); j++) {
            char c = l[j];
            QRect charRect(X_OFFSET + (j * charWidth), yCoord, charWidth, charHeight);
            painter.drawText(charRect, Qt::AlignCenter, QString(c));
        }
        yCoord += charHeight; // Move to the next line
    } 
    
    // cursorIndex is up to date, just find where to draw it
    int cursorLine = -1;
    int cursorChar = 0;
    int totalChars = 0;
     for (int i = 0; i<lines.size(); i++) {
         std::string & l = lines[i];
         int lSize = l.size();

        if ( (totalChars + lSize) >= cursorIndex) {
            cursorLine = i;
            cursorChar = cursorIndex - totalChars;
            break;
        }
        totalChars += lSize;

        char c = textBuffer->charAt(totalChars);
        if (c == '\n') {totalChars+=1;}
     }
     if (cursorLine == -1) {
        cursorLine = lines.size();
     }
     
    int cursorX = X_OFFSET + (cursorChar * charWidth);
    int cursorY = Y_OFFSET + (cursorLine * charHeight) - verticalScrollOffset;


    painter.setPen(Qt::red);
    painter.drawLine(cursorX, cursorY, cursorX, cursorY + charHeight);

}

void TextEditor::wheelEvent(QWheelEvent *event) {
    int delta = event->angleDelta().y(); 

    verticalScrollOffset -= (delta/120) * charHeight;
    if (verticalScrollOffset < 0) {verticalScrollOffset = 0;}

    
    int maxOffset = charHeight * lines.size();
    if (verticalScrollOffset > maxOffset) {verticalScrollOffset = maxOffset;}

    
    update(); 
}

void TextEditor::keyPressEvent(QKeyEvent * event) {
    // sync cursor
    int key = event->key();

   
    int maxWidth = (width() - X_OFFSET) / charWidth;
    // sync cursor
    this->textBuffer->moveCursor(cursorIndex);

    switch (event->key()) {
        case Qt::Key_Shift:
        case Qt::ALT:
        case Qt::Key_Tab:
            return;

        case Qt::Key_Backspace:
            this->textBuffer->backspace();
            this->moveCursorIndex(-1);
            break;
        case Qt::Key_Delete:
            this->textBuffer->delete_after();
            update();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            this->textBuffer->insert('\n');
            this->moveCursorIndex(1);
            break;

        default:
            bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
            if (!shiftPressed && (key>=65 && key <= 90)) {
                key += 32;
            }
            char c = key;
            this->textBuffer->insert(c);
            this->moveCursorIndex(1);
            
            break;
    }


}

void TextEditor::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {return;}
    // QPointF pos = event->position();
    int x = event->x();
    int y = event->y() + verticalScrollOffset;
    if (x < X_OFFSET || y < Y_OFFSET) {return;}
    // determine if its left or right of the char
    

    int clickedLine = (y-Y_OFFSET) / charHeight;
    if (clickedLine < 0) {
        cursorIndex = 0;
        update();
        return;
        
    }
    if (clickedLine >=lines.size()) {
        cursorIndex = textBuffer->length();
        update();
        return;
    }

    const std::string &line = lines[clickedLine];
    int clickedChar = (x-X_OFFSET)/charWidth;
    if (clickedChar < 0) {clickedChar = 0;}
    else if (clickedChar >= line.size()) {
        clickedChar = line.size();
    }

    int cpt = 0;
    for (int i = 0; i < clickedLine; i++) {
        int size = lines[i].size();
        cpt += size;
        char c = textBuffer->charAt(cpt);
        if (c == '\n') {cpt+=1;}
    }
    cpt += clickedChar;
    cursorIndex = cpt;

    this->textBuffer->moveCursor(cursorIndex);
    
    update();

}



void TextEditor::moveCursorIndex(int delta) {
    cursorIndex += delta;
    if (cursorIndex < 0) {cursorIndex = 0;}
    int l = this->textBuffer->length();
    if (cursorIndex > l) {cursorIndex = l;}
    update();
}

void TextEditor::moveOneLineUp() {
    int totalChars = 0;
    for (int i = 0; i<lines.size()-1; i++) {         
        std::string & l = lines[i];

        int sizeLine = l.size();        
        char c = textBuffer->charAt(totalChars+sizeLine);
        if (c == '\n') {sizeLine+=1;}
        totalChars += sizeLine;

        if (i == 0 && totalChars >= cursorIndex) {
            // if we were on first line, get to the beginning of it
            cursorIndex = 0;
            break;
        }

        int totalAtNextLine = totalChars + lines[i+1].size();
        if ( totalAtNextLine >= cursorIndex) { // if i+1 has the cursor, then i is the new line
            // i is the new line
            int cursorChar = (cursorIndex - totalChars);
            if (cursorChar > sizeLine) {
                cursorChar = l.size();
            }
            cursorIndex = (totalChars - sizeLine) + cursorChar;
            
            break;
        }    
    }
    update();
}

void TextEditor::moveOneLineDown() {
    int totalChars = 0;
    for (int i = 0; i<lines.size(); i++) {
         std::string & l = lines[i];
         int sizeLine = l.size();        
         char c = textBuffer->charAt(totalChars+sizeLine);
         if (c == '\n') {sizeLine+=1;}

        if ( (totalChars + l.size()) >= cursorIndex) {
            // cursor is in line i
            if (i == (lines.size() - 1)) { // if its last line then move cursor to the very end
                cursorIndex = this->textBuffer->length();
                break;
            }

            int cursorChar = cursorIndex - totalChars;
            if (cursorChar > lines[i+1].size()) {
                cursorChar = lines[i+1].size();
            }
            cursorIndex = totalChars + sizeLine + cursorChar;
            break;
        }
        totalChars += sizeLine;
     }

    update();

}

TextEditor::TextEditor()
{
    // Dummy data to try the paint function
    char * initText = "Hello !\nThis is a test text. Here comes a long sentence to try out automatic line wrapping when words go out of bounds";
    // textBuffer = new TextBuffer(initText, strlen(initText));
    textBuffer = new TextBuffer("", 0);
    cursorIndex = 0;
    cursorEndIndex = cursorIndex;
    
    this->setObjectName("TextEditor");
    QColor originalColor = palette().color(QPalette::Base);
    setStyleSheet(QString("#TextEditor { background-color: %1 }").arg(originalColor.name())); 
    
    // setupWelcomeScreen(this);

    setCursor(Qt::IBeamCursor);
    // give focus to this widget
    setFocusPolicy(Qt::StrongFocus);




    QShortcut * rightArrow = new QShortcut(QKeySequence::MoveToNextChar, this);
    connect(rightArrow, &QShortcut::activated, this, [this]() {
        this->moveCursorIndex(1);
        });
        
    QShortcut * leftArrow = new QShortcut(QKeySequence::MoveToPreviousChar, this);
    connect(leftArrow, &QShortcut::activated, this, [this]() {
        this->moveCursorIndex(-1);
    });

    QShortcut * upArrow = new QShortcut(QKeySequence::MoveToPreviousLine, this);
    connect(upArrow, &QShortcut::activated, this, [this]() {
        this->TextEditor::moveOneLineUp();
    });

    QShortcut * downArrow = new QShortcut(QKeySequence::MoveToNextLine, this);
    connect(downArrow, &QShortcut::activated, this, [this]() {
        this->TextEditor::moveOneLineDown();
    });

    // Bind CTRL+S to save slot
    QShortcut * saveShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    saveShortcut->setAutoRepeat(false); // so it doesnt spam trigger when we keep it pressed
    connect(saveShortcut, &QShortcut::activated, this, &TextEditor::saveFileTriggered);

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
    
}

