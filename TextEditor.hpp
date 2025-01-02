#pragma once

#include <QTextEdit>
#include <QWidget>
#include <QString>

class TextEditor: public QWidget {
    Q_OBJECT
    private:
        QTextEdit * textEdit;
    public:
        TextEditor();
    signals:
        void saveFileWithContent(QString);
    public slots:
        void fileHasBeenOpened(QString &);
        void saveFileTriggered();

    
};