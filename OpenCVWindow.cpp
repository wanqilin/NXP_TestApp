#include "OpenCVWindow.h"
#include <QtDebug>

OpenCVWindow::OpenCVWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug()<<"IntoOpenCVWindow!";
    setWindowTitle(tr("OpenCV"));
    resize(400, 300);
}

OpenCVWindow::~OpenCVWindow() {}
