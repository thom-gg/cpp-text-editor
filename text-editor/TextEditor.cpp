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

    QShortcut * undoShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), this);
    connect(undoShortcut, &QShortcut::activated, this, &TextEditor::undoAction);

    QShortcut *redoShortcut = new QShortcut(QKeySequence::Redo, this);
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
}

