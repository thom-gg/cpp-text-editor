#include <QApplication>
#include "MainWindow.hpp"
#include <QTextEdit>
#include <QStyleFactory>
#include <QPalette>

int main(int argc, char **argv)
{
 QApplication app (argc, argv);


QPalette palette;
palette.setColor(QPalette::Window, QColor("#2B2B2B"));      // Background color
palette.setColor(QPalette::WindowText, QColor("white")); // Text color
palette.setColor(QPalette::Base, QColor("#303841"));       // Input fields, etc.
palette.setColor(QPalette::Text, QColor("#D8DEE9"));       // Text in input fields
palette.setColor(QPalette::AlternateBase, QColor("green"));

// Apply the palette globally
app.setPalette(palette);

 app.setStyleSheet("QMenuBar { background-color: black; color: white; }");

 MainWindow window;


 window.show();



 return app.exec();
}