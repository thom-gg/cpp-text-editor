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
#include <QGuiApplication>
#include <QClipboard>

#include "SillyCat.hpp"
#include "CatFactory.hpp"

#define DOUBLE_CLICK_DELAY 500 // max delay after which its not considered a double click but two clicks

void TextEditor::fileHasBeenOpened(QString &content)
{
    if (textBuffer != nullptr) {
        delete textBuffer;
    }
    std::string stdString = content.toStdString();
    
    textBuffer = new TextBuffer(stdString.data(), stdString.size());
    cursorIndex = stdString.size();
    
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


void TextEditor::updateZooming(int amount)
{
    amount *= 3;
    fontSize += amount;
    if (fontSize < 1) {fontSize = 1;}
    update();
}



void TextEditor::printNewLine(int lineIndex, int & linesNumber, QPainter &painter)
{
    linesNumber += 1; // draw line number
    painter.setPen(Qt::gray);
    painter.setFont(QFont("Courier", fontSize*0.80));
    painter.drawText(QRect(0, (Y_OFFSET + (lineIndex) * charHeight) - verticalScrollOffset, charWidth*2, charHeight), Qt::AlignLeft | Qt::AlignVCenter, QString("%1:").arg(linesNumber));
    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier", fontSize));

}

void TextEditor::paintEvent(QPaintEvent *event) {
    if (this->textBuffer == nullptr) {return;}
    QStyleOption opt;
    opt.init(this);  
    QPainter painter(this);
    // drawing the style sheet before doing our custom painting event (used for background color painting (save file blink for example))
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    painter.setPen(Qt::white);
    // using a monospace font so we can count the width for each character and know where to break line
    painter.setFont(QFont("Courier", fontSize));
    
    lines.clear();


    QRect bounds = painter.boundingRect(rect(), Qt::AlignLeft, "A");
    charWidth = bounds.width();
    X_OFFSET = 3* charWidth;
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

   
    
    
    int * cursorPos = findCursorPosition(cursorIndex);
    if (cursorPos[1] < 0 && syncVerticalOffset) {
        // should scroll up automatically
        syncVerticalOffset = false;
        verticalScrollOffset -= 2* charHeight + (0 - cursorPos[1]);
        update();
    }
    else if (( cursorPos[1]+charHeight) > maxHeight && syncVerticalOffset) {        
        // should scroll down automatically
        syncVerticalOffset = false;
        verticalScrollOffset += 2 *charHeight + (cursorPos[1]+charHeight - maxHeight);
        update();
    }
    if (cursorEndIndex != cursorIndex) {
        int * cursorEndPos = findCursorPosition(cursorEndIndex);
        this->drawSelection(cursorPos, cursorEndPos, cursorEndIndex, painter);
        delete [] cursorEndPos;
    }
    else {
        painter.setPen(Qt::red);
        painter.drawLine(cursorPos[0], cursorPos[1], cursorPos[0], cursorPos[1] + charHeight);
        painter.setPen(Qt::white); 
    }

    
    delete [] cursorPos;

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
}

void TextEditor::drawSelection(int * cursorPos, int *  cursorEndPos, int cursorEndIndex, QPainter &painter) {
        int * minPos = cursorPos;
        int * maxPos = cursorEndPos;
        if (cursorEndIndex < cursorIndex) {
            minPos = cursorEndPos;
            maxPos = cursorPos;
        }
   
        for (int line = minPos[1]; line <= maxPos[1]; line+= charHeight) {
            if (line == minPos[1] && line == maxPos[1]) {
                // draw from minPos[0] to maxPos[0]
                painter.fillRect(QRect(minPos[0], line, maxPos[0] - minPos[0], charHeight), "blue");
            }
            else if (line == minPos[1]) {
                // draw from minPos[0] to end of line
                painter.fillRect(QRect(minPos[0], line, width()  - minPos[0], charHeight), "blue");

            }
            else if (line == maxPos[1]) {
                // draw from beginning of line (X_OFFSET) to maxPos[0] 
                painter.fillRect(QRect(X_OFFSET, line, maxPos[0] - X_OFFSET, charHeight), "blue");
            }
            else {
                // draw full line
                painter.fillRect(QRect(X_OFFSET, line, width(), charHeight), "blue");
            }
        }
}

// can be used to find both cursor position and cursorEnd position
int * TextEditor::findCursorPosition(int index) {
    int cursorLine = -1;
    int cursorChar = 0;
    int totalChars = 0;
     for (int i = 0; i<lines.size(); i++) {
         std::string & l = lines[i];
         int lSize = l.size();

        if ( (totalChars + lSize) >= index) {
            cursorLine = i;
            cursorChar = index - totalChars;
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
    int * res = new int[2]; // up to the caller to free this variable
    res[0] = cursorX;
    res[1] = cursorY;
    return res;
}

void TextEditor::wheelEvent(QWheelEvent *event) {
    int delta = event->angleDelta().y(); 
    bool ctrlPressed = event->modifiers() & Qt::ControlModifier;
    if (ctrlPressed) {
        this->updateZooming(delta/120);
        return;
    }

    verticalScrollOffset -= (delta/120) * charHeight;
    if (verticalScrollOffset < 0) {verticalScrollOffset = 0;}

    
    int maxOffset = charHeight * lines.size();
    if (verticalScrollOffset > maxOffset) {verticalScrollOffset = maxOffset;}

    
    update(); 
}

void TextEditor::keyPressEvent(QKeyEvent * event) {
    // sync cursor
    int key = event->key();
    bool shiftPressed = event->modifiers() & Qt::ShiftModifier;

    int maxWidth = (width() - X_OFFSET) / charWidth;
    // sync cursor
    this->textBuffer->moveCursor(cursorIndex);
    syncVerticalOffset = true;

    switch (event->key()) {
        case Qt::Key_Shift:
        case Qt::Key_Alt:
        case Qt::Key_Tab:
        case Qt::Key_Control:
            return;
        
        case Qt::Key_Left:
        case Qt::Key_Right:
            if (shiftPressed) {
                cursorEndIndex += key == Qt::Key_Left ? -1 : 1;
                if (cursorEndIndex < 0) {cursorEndIndex = 0;}     
                int l = this->textBuffer->length();
                if (cursorEndIndex > l) {cursorEndIndex = l;}       
                update();
            }

            break;
        case Qt::Key_Up:
            if (shiftPressed) {
                this->moveOneLineUp(true);
            }
            break;
        case Qt::Key_Down:
            if (shiftPressed) {
                this->moveOneLineDown(true);
            }
            break;
        case Qt::Key_Backspace:
            if (cursorIndex != cursorEndIndex) {
                this->deleteSelection();
                update();
            }
            else {
                this->textBuffer->backspace();
                this->moveCursorIndex(-1);
            }
            emit signalFileHasBeenModified();
            syncVerticalOffset = true;


            break;
        case Qt::Key_Delete:
            this->textBuffer->delete_after();
            emit signalFileHasBeenModified();
            syncVerticalOffset = true;
            update();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (cursorIndex != cursorEndIndex) {
                this->deleteSelection();
            }
            this->textBuffer->insert('\n');
            syncVerticalOffset = true;
            emit signalFileHasBeenModified();
            this->moveCursorIndex(1);
            break;

        default:
            if (cursorIndex != cursorEndIndex) {
                this->deleteSelection();
            }
            syncVerticalOffset = true;
            if (!shiftPressed && (key>=65 && key <= 90)) {
                key += 32;
            }
            char c = key;
            this->textBuffer->insert(c);
            this->moveCursorIndex(1);
            emit signalFileHasBeenModified();

            break;
    }


}

int TextEditor::findCursorIndexForPos(int x, int y) {
    int clickedLine = (y-Y_OFFSET) / charHeight;
    if (clickedLine < 0) {
        return 0;
        
    }
    if (clickedLine >=lines.size()) {
        return textBuffer->length();
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
    return cpt;
}

bool isAlphaNumerical(char c) {
    return (c >= 'a' && c<='z') || (c >= 'A' && c<= 'Z') || (c >= '0' && c <= '9');
}

void TextEditor::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {return;}
    qint64 timestamp = event->timestamp();
    qint64 diff = lastClick == 0 ? 0 : timestamp - lastClick;
    lastClick = timestamp; // store last click, needed to detect double clicks

    int x = event->x();
    int y = event->y() + verticalScrollOffset;
    if (x < X_OFFSET || y < Y_OFFSET) {return;}    

    int oldIndex = cursorIndex;
    cursorIndex = findCursorIndexForPos(x,y);
    cursorEndIndex = cursorIndex;
    if (diff < DOUBLE_CLICK_DELAY && oldIndex == cursorIndex && isAlphaNumerical(textBuffer->charAt(cursorIndex))) {
        // select the whole word cursorIndex is in
        int left = cursorIndex;
        while (isAlphaNumerical(textBuffer->charAt(left))) {left-=1;}
        int right = cursorIndex;
        while (isAlphaNumerical(textBuffer->charAt(right))) {right+=1;}
        cursorEndIndex = left+1;
        cursorIndex=right;
    }
    else {
        isLeftClickPressed = true;

        this->textBuffer->moveCursor(cursorIndex); 
    }
   
    
    
    update();

}

 void TextEditor::mouseMoveEvent(QMouseEvent * event) {
    if (!isLeftClickPressed) {return;}
    int x = event->x();
    int y = event->y() + verticalScrollOffset;

    cursorEndIndex = findCursorIndexForPos(x,y);
    update();
 };

void TextEditor::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {return;}
    if (!isLeftClickPressed) {return;}
    int x = event->x();
    int y = event->y() + verticalScrollOffset;
    if (x < X_OFFSET || y < Y_OFFSET) {return;}    

    cursorEndIndex = findCursorIndexForPos(x,y);
    isLeftClickPressed = false;
    update();
        
};

void TextEditor::moveCursorIndex(int delta) {
    // get out of text selection
    syncVerticalOffset = true;

    if (cursorIndex != cursorEndIndex) {
        if (delta < 0) {
            cursorIndex = std::min(cursorIndex, cursorEndIndex);
            cursorEndIndex = cursorIndex;
        }
        else {
            cursorIndex = std::max(cursorIndex, cursorEndIndex);
            cursorEndIndex = cursorIndex;
        }
        update();
        return;
    }
    // regular behavior
    cursorIndex += delta;
    if (cursorIndex < 0) {cursorIndex = 0;}
    int l = this->textBuffer->length();
    if (cursorIndex > l) {cursorIndex = l;}
    cursorEndIndex = cursorIndex;
    update();
}

void TextEditor::moveOneLineUp(bool movingSelection = false) {
    syncVerticalOffset = true;

    int index = movingSelection ? cursorEndIndex : cursorIndex;
    int totalChars = 0;
    for (int i = 0; i<lines.size()-1; i++) {        
        std::string & l = lines[i];

        int sizeLine = l.size();        
        char c = textBuffer->charAt(totalChars+sizeLine);
        if (c == '\n') {sizeLine+=1;}
        totalChars += sizeLine;

        if (i == 0 && totalChars > index) {
            // if we were on first line, get to the beginning of it
            index = 0;
            break;
        }

        int totalAtNextLine = totalChars + lines[i+1].size();
        if ( totalAtNextLine >= index) { // if i+1 has the cursor, then i is the new line
            // i is the new line
            int cursorChar = (index - totalChars);
            if (cursorChar > sizeLine) {
                cursorChar = l.size();
            }
            index = (totalChars - sizeLine) + cursorChar;
            
            break;
        }    
    }
    if (!movingSelection) {
        cursorIndex = index;
        cursorEndIndex = cursorIndex;
    }
    else {
        cursorEndIndex = index;
    }
    update();
}

void TextEditor::moveOneLineDown(bool movingSelection = false) {
    syncVerticalOffset = true;

    int index = movingSelection ? cursorEndIndex : cursorIndex;
    int totalChars = 0;
    for (int i = 0; i<lines.size(); i++) {
         std::string & l = lines[i];
         int sizeLine = l.size();        
         char c = textBuffer->charAt(totalChars+sizeLine);
         if (c == '\n') {sizeLine+=1;}

        if ( (totalChars + l.size()) >= index) {
            // cursor is in line i
            if (i == (lines.size() - 1)) { // if its last line then move cursor to the very end
                index = this->textBuffer->length();
                break;
            }

            int cursorChar = index - totalChars;
            if (cursorChar > lines[i+1].size()) {
                cursorChar = lines[i+1].size();
            }
            index = totalChars + sizeLine + cursorChar;
            break;
        }
        totalChars += sizeLine;
     }
    if (!movingSelection) {
        cursorIndex = index;
        cursorEndIndex = cursorIndex;
    }
    else {
        cursorEndIndex = index;
    }
    update();

}

void TextEditor::copySelection() {
    if (cursorIndex == cursorEndIndex) {
        return;
    }
    int start = cursorIndex < cursorEndIndex ? cursorIndex : cursorEndIndex;
    int end = cursorIndex < cursorEndIndex ? cursorEndIndex : cursorIndex;
    int size = end-start;
    char * selectedText = new char[size+1];
    selectedText[size] = '\0';
    for (int i = 0; i<size; i++) {
        selectedText[i] = this->textBuffer->charAt(start+i);
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(selectedText);

    delete [] selectedText;

}

void TextEditor::deleteSelection() {
    // remove selection, put cursor on the left then paste
    int start = cursorIndex < cursorEndIndex ? cursorIndex : cursorEndIndex;
    int end = cursorIndex < cursorEndIndex ? cursorEndIndex : cursorIndex;
    this->textBuffer->deleteSelection(start, end);
    cursorIndex = start;
    cursorEndIndex = start;
}

void TextEditor::paste() {
    syncVerticalOffset = true;

    if (cursorIndex != cursorEndIndex) {
        this->deleteSelection();
    }

    // Insert content of clipboard
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString text =  clipboard->text();
    for (int i = 0; i<text.length(); i++) {
        char c = text.at(i).toLatin1();
        this->textBuffer->insert(c);
        cursorIndex += 1;
    }

    cursorEndIndex = cursorIndex;
    
    emit signalFileHasBeenModified();
    update();
}

void TextEditor::selectAll() {
    cursorEndIndex = 0;
    cursorIndex = this->textBuffer->length();
    update();
}

TextEditor::TextEditor()
{
    
    // textBuffer = new TextBuffer("", 0);
    // cursorIndex = 0;
    // cursorEndIndex = cursorIndex;
    
    this->setObjectName("TextEditor");
    QColor originalColor = palette().color(QPalette::Base);
    setStyleSheet(QString("#TextEditor { background-color: %1 }").arg(originalColor.name())); 
    
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


    // Bind CTRL+C to copy
    QShortcut * copyShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);
    copyShortcut->setAutoRepeat(false);
    connect(copyShortcut, &QShortcut::activated, this, &TextEditor::copySelection);

    QShortcut * pasteShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), this);
    pasteShortcut->setAutoRepeat(false);
    connect(pasteShortcut, &QShortcut::activated, this, &TextEditor::paste);

    QShortcut * selectAllShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_A), this);
    selectAllShortcut->setAutoRepeat(false);
    connect(selectAllShortcut, &QShortcut::activated, this, &TextEditor::selectAll);

    QShortcut * zoomInShortcut = new QShortcut(QKeySequence::ZoomIn, this);
    connect(zoomInShortcut, &QShortcut::activated, this, [this]() {
        updateZooming(1);
    });

    QShortcut * zoomOutShortcut = new QShortcut(QKeySequence::ZoomOut, this);
    connect(zoomOutShortcut, &QShortcut::activated, this, [this]() {
        updateZooming(-1);
    });

    // waiting for next event loop iteration (so that the widget is fully constructed) before emitting this signal
    QTimer::singleShot(0, this, &TextEditor::newEmptyFileRequested);


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

