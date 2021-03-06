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

    ///new map was loaded, inits managers, priority, goalPos and onePlaceCounter value
    void NewMap();

    ///shows information in UI
    void UpdateUI();

    ///updates data stores and processed by robotmanager and mapmanager
    void UpdateManagers();

    ///move sequence
    bool Move();

    ///looks for finish line
    bool LookForFinishLine();

    ///possible moves
    void Rush();
    void MoveFastForward();
    void MoveForward();
    void Wait();
    void RotateLeft();
    void RotateRight();

    void new_message();

    void client_disconnected();

    ///chooses in which direction robot schould go
    char ChooseDirection();

    /// Checks if current priority schould become true
    void CheckPriority();

    ///sets priorityLeft true
    void setPriorityLeft();

    ///sets priorityLeft False (highest priority has right direction)
    void resetPriorityLeft();

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QString localMap;
    QTextStream socketStream;
    RobotManager robotManager; ///< object used to manage robot position on the map
    MapManager mapManager;     ///< object used to manage maps
    int onePlaceCounter;       ///< counts how many times robot stays in the same place
    bool priorityLeft = false; ///< become true when finish line was found on the left
    std::vector<int> goalPos;            ///<position on the global map of the end tile if it's known (our goal is to get there)


};

#endif // MAINWINDOW_H
