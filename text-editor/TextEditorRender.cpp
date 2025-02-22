#include "TextEditor.hpp"

// #######
// Holds all the rendering logic
// Painting the editor content to the screen
// While handling zoom levels, text selection, cursor position
// #######



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



void TextEditor::updateZooming(int amount)
{
    amount *= 3;
    fontSize += amount;
    if (fontSize < 1) {fontSize = 1;}
    update();
}