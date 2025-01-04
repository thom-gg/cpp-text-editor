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
    TextEditor.cpp \
    ./fun/SillyCat.cpp \
    ./fun/CatFactory.cpp

HEADERS += \
    MainWindow.hpp \
    CustomMenu.hpp \
    FileManager.hpp \
    TextEditor.hpp  \
    ./fun/SillyCat.hpp \
    ./fun/CatFactory.hpp


# Qt modules
QT += core gui widgets




