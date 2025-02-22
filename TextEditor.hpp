#pragma once

#include <QTextEdit>
#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QWheelEvent>
#include "./buffer/TextBuffer.hpp"

class CatFactory; // for circular dependencies

class TextEditor : public QWidget
{
    Q_OBJECT
private:
    qint64 lastClick = 0;
    int fontSize = 20;

    int charWidth = 0;
    int charHeight = 0;

    bool isLeftClickPressed = false;
    bool syncVerticalOffset = false; // this is set to true when vertical offset should be synchronized with cursor (when a key is pressed usually)

    int cursorIndex;
    int cursorEndIndex; // should be equal to cursorIndex, except when doing a text selection
    int X_OFFSET = 50;
    int Y_OFFSET = 0;
    std::vector<std::string> lines;

    int verticalScrollOffset = 0;

    CatFactory *factory = nullptr;
    TextBuffer *textBuffer = nullptr;
    void setupWelcomeScreen(QWidget *);
    void updateZooming(int amount);
    void setupQTextEdit();
    void printNewLine(int , int &, QPainter &painter);
    void moveCursorIndex(int delta);
    void moveOneLineUp(bool movingSelection);
    void moveOneLineDown(bool movingSelection);

    void deleteSelection();

    void copySelection();
    void paste();

    void selectAll();

    int findCursorIndexForPos(int x, int y);

    int * findCursorPosition(int index);
    void drawSelection(int * cursorPos, int *  cursorEndPos, int cursorEndIndex, QPainter & painter);

public:
    TextEditor();
    ~TextEditor();
signals:
    void signalFileHasBeenModified(); // to be caught by MainWindow to update window title
    void saveFileWithContent(std::string);
    void openFileRequested();
    void newEmptyFileRequested(); // sending to FileManager that we are creating an empty file
public slots:
    void fileHasBeenOpened(std::string &);
    void saveFileTriggered();
    void newEmptyFile(); // receiving from CustomMenu that we need to create a new empty file

protected:
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;


};