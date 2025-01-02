#pragma once

#include <QTextEdit>
#include <QWidget>
#include <QString>
#include <QVBoxLayout>

class TextEditor: public QWidget {
    Q_OBJECT
    private:
        QTextEdit * textEdit = nullptr;
        void setupWelcomeScreen(QWidget *);
    public:
        TextEditor();
    signals:
        void saveFileWithContent(QString);
        void openFileRequested();
    public slots:
        void fileHasBeenOpened(QString &);
        void saveFileTriggered();

    
};