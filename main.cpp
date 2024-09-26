#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Creat font object,Set font and size
    QFont font("Arial",12);
    //set app global font
    a.setFont(font);

    MainWindow w;
    w.show();
    return a.exec();
}
