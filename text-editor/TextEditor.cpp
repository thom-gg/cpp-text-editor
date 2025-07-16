#include "TextEditor.hpp"

// Main TextEditor file, contains constructor / destructor
TextEditor::TextEditor()
{
        
    this->setObjectName("TextEditor");
    QColor originalColor = palette().color(QPalette::Base);
    setStyleSheet(QString("#TextEditor { background-color: %1 }").arg(originalColor.name())); 
    
    setCursor(Qt::IBeamCursor);
    // give focus to this widget
    setFocusPolicy(Qt::StrongFocus);

    // Init shortcuts and link them to appropriate actions
    rightArrow = new QShortcut(QKeySequence::MoveToNextChar, this);
    connect(rightArrow, &QShortcut::activated, this, [this]() {
        this->moveCursorIndex(1);
        });
        
    leftArrow = new QShortcut(QKeySequence::MoveToPreviousChar, this);
    connect(leftArrow, &QShortcut::activated, this, [this]() {
        this->moveCursorIndex(-1);
    });

    upArrow = new QShortcut(QKeySequence::MoveToPreviousLine, this);
    connect(upArrow, &QShortcut::activated, this, [this]() {
        this->TextEditor::moveOneLineUp();
    });

    downArrow = new QShortcut(QKeySequence::MoveToNextLine, this);
    connect(downArrow, &QShortcut::activated, this, [this]() {
        this->TextEditor::moveOneLineDown();
    });

    // Bind CTRL+S to save slot
    saveShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    saveShortcut->setAutoRepeat(false); // so it doesnt spam trigger when we keep it pressed
    connect(saveShortcut, &QShortcut::activated, this, &TextEditor::saveFileTriggered);


    // Bind CTRL+C to copy
    copyShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);
    copyShortcut->setAutoRepeat(false);
    connect(copyShortcut, &QShortcut::activated, this, &TextEditor::copySelection);

    pasteShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), this);
    pasteShortcut->setAutoRepeat(false);
    connect(pasteShortcut, &QShortcut::activated, this, &TextEditor::paste);

    selectAllShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_A), this);
    selectAllShortcut->setAutoRepeat(false);
    connect(selectAllShortcut, &QShortcut::activated, this, &TextEditor::selectAll);

    zoomInShortcut = new QShortcut(QKeySequence::ZoomIn, this);
    connect(zoomInShortcut, &QShortcut::activated, this, [this]() {
        updateZooming(1);
    });

    zoomOutShortcut = new QShortcut(QKeySequence::ZoomOut, this);
    connect(zoomOutShortcut, &QShortcut::activated, this, [this]() {
        updateZooming(-1);
    });

    undoShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), this);
    connect(undoShortcut, &QShortcut::activated, this, &TextEditor::undoAction);

    redoShortcut = new QShortcut(QKeySequence::Redo, this);
    connect(redoShortcut, &QShortcut::activated, this, &TextEditor::redoAction);

    // waiting for next event loop iteration (so that the widget is fully constructed) before emitting this signal
    QTimer::singleShot(0, this, &TextEditor::newEmptyFileRequested);


    this->undoStack = new Stack<UndoEvent *>();
    this->redoStack = new Stack<UndoEvent *>();

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
    if (undoStack != nullptr) {
        delete undoStack;
    }
    if (redoStack != nullptr) {
        delete redoStack;
    }
    delete rightArrow;
    delete leftArrow;
    delete upArrow;
    delete downArrow;
    delete saveShortcut;
    delete copyShortcut;
    delete pasteShortcut;
    delete selectAllShortcut;
    delete zoomInShortcut;
    delete zoomOutShortcut;
    delete undoShortcut;
    delete redoShortcut;
 
}

