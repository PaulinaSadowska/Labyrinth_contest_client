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



private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QTextStream socketStream;
    RobotManager robotManager;
    MapManager mapManager;
    int onePlaceCounter;
    int sawFinishLine = 0; ///< flag, 1-finish line on the right, 2-finish line on the left


};

#endif // MAINWINDOW_H
