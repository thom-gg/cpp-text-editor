#pragma once

#include <QTextEdit>
#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QWheelEvent>

class TextEditor: public QWidget {
    Q_OBJECT
    private:
        QTextEdit * textEdit = nullptr;
        void setupWelcomeScreen(QWidget *);
        void updateZooming(int amount);

    public:
        TextEditor();
    signals:
        void saveFileWithContent(QString);
        void openFileRequested();
        void newEmptyFileRequested(); // sending to FileManager that we are creating an empty file
    public slots:
        void fileHasBeenOpened(QString &);
        void saveFileTriggered();
        void newEmptyFile(); // receiving from CustomMenu that we need to create a new empty file

    protected:
        void wheelEvent(QWheelEvent *event) override;
};