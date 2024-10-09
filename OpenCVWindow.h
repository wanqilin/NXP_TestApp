#ifndef OPENCVWINDOW_H
#define OPENCVWINDOW_H
#include <QMainWindow>

class OpenCVWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit OpenCVWindow(QWidget *parent = nullptr);
    ~OpenCVWindow();
};

#endif // OPENCVWINDOW_H
