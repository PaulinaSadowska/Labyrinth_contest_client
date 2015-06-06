#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QTextStream>
#include "RobotManager.h"
#include "MapManager.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButtonRush_clicked();

    void on_pushButtonFastForward_clicked();

    void on_pushButtonForward_clicked();

    void on_pushButtonWait_clicked();

    void on_pushButtonRotateLeft_clicked();

    void on_pushButtonRotateRight_clicked();

    bool Move();
    bool LookForFinishLine();

    void Rush();

    void MoveFastForward();

    void MoveForward();

    void Wait();

    void RotateLeft();

    void RotateRight();

    void new_message();

    void client_disconnected();

   /* bool Step00();
    bool Step10();
    bool Step20();
    bool Step01();
    bool Step02();
    bool Step12();
    bool Step03();
    bool Step04();
    bool Step13();*/


private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QTextStream socketStream;
    RobotManager robotManager;
    MapManager mapManager;
    QChar nearMap[28];
    bool doneFlag;

    //enum {Right, Forward, Left, Whatever} nextMove;

};

#endif // MAINWINDOW_H
