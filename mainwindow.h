#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void RecvTimer();

private slots:
    void PrintText(const QString &text);

private:
    QLineEdit *inputTxt1;
    QLCDNumber *lcdnumber;
    QLabel *displayTxt1;
    QTimer *m_timer;
    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
};
#endif // MAINWINDOW_H
