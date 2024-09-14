#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void PrintText(const QString &text);
private:
    QLineEdit *inputTxt1;
    QLabel *displayTxt1;
    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
};
#endif // MAINWINDOW_H
