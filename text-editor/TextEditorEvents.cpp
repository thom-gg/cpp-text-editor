#include "TextEditor.hpp"

// #######
// Events listener for keyboard / mouse inputs
// Mainly dispatch tasks to core logic components (including TextBuffer)
// #######

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