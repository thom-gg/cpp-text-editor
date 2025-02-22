# Project name
TEMPLATE = app
TARGET = textEditor

INCLUDEPATH += ./fun

# Sources and headers
SOURCES += \
    main.cpp \
    MainWindow.cpp \
    CustomMenu.cpp \
    FileManager.cpp \
    ./text-editor/TextEditor.cpp \
    ./text-editor/TextEditorRender.cpp \
    ./text-editor/TextEditorFile.cpp \
    ./text-editor/TextEditorEvents.cpp \
    ./text-editor/TextEditorCursor.cpp \
    ./fun/SillyCat.cpp \
    ./fun/CatFactory.cpp \
    ./buffer/TextBuffer.cpp

HEADERS += \
    MainWindow.hpp \
    CustomMenu.hpp \
    FileManager.hpp \
    ./text-editor/TextEditor.hpp  \
    ./fun/SillyCat.hpp \
    ./fun/CatFactory.hpp \
    ./buffer/TextBuffer.hpp


# Qt modules
QT += core gui widgets




