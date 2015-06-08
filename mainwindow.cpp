#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),socket(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket=new QTcpSocket(this);

    //local map table
    for(int i=0;i<7;i++)
        for(int j=0;j<4;j++)
            ui->robotVisionTable->setItem(j,i,new QTableWidgetItem("?"));
    for(int i=0;i<7;i++)
        ui->robotVisionTable->setColumnWidth(i,30);
    for(int i=0;i<4;i++)
        ui->robotVisionTable->setRowHeight(i,30);

    //nearest map table
    for(int i=0;i<7;i++)
        for(int j=0;j<4;j++)
            ui->nearestMapTable->setItem(j,i,new QTableWidgetItem("?"));
    for(int i=0;i<7;i++)
        ui->nearestMapTable->setColumnWidth(i,30);
    for(int i=0;i<4;i++)
        ui->nearestMapTable->setRowHeight(i,30);

    //global map table
    for(int i=0;i<99;i++)
        for(int j=0;j<99;j++)
            ui->globalMapTable->setItem(j,i,new QTableWidgetItem(" "));
    for(int i=0;i<99;i++)
        ui->globalMapTable->setColumnWidth(i,20);
    for(int i=0;i<99;i++)
        ui->globalMapTable->setRowHeight(i,20);

    onePlaceCounter = 0;
}

MainWindow::~MainWindow()
{
    socket->close();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    socket->abort();
    socket->connectToHost(ui->addressEdit->text(),ui->portEdit->text().toInt());
    if(socket->isWritable())
    {
        ui->pushButtonFastForward->setEnabled(true);
        ui->pushButtonForward->setEnabled(true);
        ui->pushButtonRush->setEnabled(true);
        ui->pushButtonWait->setEnabled(true);
        ui->pushButtonRotateLeft->setEnabled(true);
        ui->pushButtonRotateRight->setEnabled(true);
        socketStream.setDevice(socket);
        connect(socket,SIGNAL(readyRead()),this,SLOT(new_message()));
        connect(socket,SIGNAL(disconnected()),this,SLOT(client_disconnected()));

    }
}

void MainWindow::on_pushButtonRush_clicked()
{
    Rush();
}

void MainWindow::on_pushButtonFastForward_clicked()
{
    MoveFastForward();
}

void MainWindow::on_pushButtonForward_clicked()
{
    MoveForward();
}

void MainWindow::on_pushButtonWait_clicked()
{
    Wait();
}

void MainWindow::on_pushButtonRotateLeft_clicked()
{
    RotateLeft();
}

void MainWindow::on_pushButtonRotateRight_clicked()
{
    RotateRight();
}

void MainWindow::new_message()
{

    if(socket->isReadable())
    {
        QString localMap;
        localMap=socketStream.readLine();
        ui->rotationEdit->setText(localMap.mid(0,1));
        ui->translationEdit->setText(localMap.mid(2,1));
        for(int i=0;i<4;i++)
            for(int j=0;j<7;j++)
                ui->robotVisionTable->item(3-i,j)->setText(localMap.mid(4+i*7+j,1));


        //update robot manager
        robotManager.MoveForward(localMap[2].digitValue());
        if(localMap[0]=='R')
            robotManager.RotateRight();
        else if(localMap[0]=='L')
            robotManager.RotateLeft();
        else if(localMap[0]=='X') //new map
        {
            robotManager.Init();
            mapManager.mapInit();
        }

        //update maps (global and nearest)
        mapManager.UpdateMap(robotManager, localMap);

        //show gobal map on ui
        for(int i=0;i<98;i++)
            for(int j=0;j<98;j++)
                ui->globalMapTable->item(i,j)->setText(mapManager.getGlobalMapElementStr(j, i));

        //show nearest map on ui
        for(int i=0;i<4;i++)
            for(int j=0;j<7;j++)
            {
                ui->nearestMapTable->item(3-i, j)->setText(mapManager.getNearestMapElementStr(j+7*i));
            }

        onePlaceCounter++;
        Move();
    }

}

bool MainWindow::Move()
{

    if(LookForFinishLine())
        return true;

    if(onePlaceCounter>2)
    {
        if(mapManager.ForwardPoints[0]>0)
        {
            MoveForward();
            onePlaceCounter=0;
            return true;
        }
    }

    if((mapManager.LeftPoints[0]==mapManager.RightPoints[0] && (mapManager.RightPoints[1]==0 || mapManager.LeftPoints[1]==0) && mapManager.ForwardPoints[0]<0) ||
            (mapManager.LeftPoints[0]==mapManager.ForwardPoints[0] && mapManager.LeftPoints[1]==0) ||
            (mapManager.RightPoints[0]==mapManager.ForwardPoints[0] && mapManager.RightPoints[1]==0))
    {
        Wait();
        return true;
    }

    if(mapManager.LeftPoints[0]<0 && mapManager.ForwardPoints[0]<0 && mapManager.RightPoints[0]<0)
    {
        RotateRight();
        return true;
    }

    int max = 0;
    char dir;

    if(mapManager.RightPoints[0]>max && mapManager.RightPoints[0]<74)
    {
        max = mapManager.RightPoints[0];
        dir = 'R';
    }
    if(mapManager.ForwardPoints[0]>max)
    {
        max = mapManager.ForwardPoints[0];
        dir = 'F';
    }
    if(mapManager.LeftPoints[0]>(max+6) && mapManager.RightPoints[0]<74)
    {
        max = mapManager.LeftPoints[0];
        dir = 'L';
    }

    switch(dir)
    {
    case 'L' :
    {
        RotateLeft();
        return true;
    }
    case 'R' :
    {
        RotateRight();
        return true;
    }
    case 'F' :
    {
        if(mapManager.ForwardPoints[0] > mapManager.ForwardPoints[1])
            MoveForward();
        else
            MoveFastForward();
        onePlaceCounter=0;
        return true;
    }
    default :
        return false;
    }

    return false;


}


bool MainWindow::LookForFinishLine()
{
    //if finish line in front of robot
    if(mapManager.getNearestMapElement(10)>100 ||
            mapManager.getNearestMapElement(17)>100 ||
            mapManager.getNearestMapElement(23)>100 ||
            mapManager.getNearestMapElement(24)>100 ||
            mapManager.getNearestMapElement(25)>100)
    {
        Rush();
        return true;
    }
    if(mapManager.getNearestMapElement(16)>100 ||
            mapManager.getNearestMapElement(18)>100 ||
            mapManager.getNearestMapElement(19)>100 ||
            mapManager.getNearestMapElement(15)>100)
    {
        MoveFastForward();
        return true;
    }
    if(mapManager.getNearestMapElement(9)>100 ||
            mapManager.getNearestMapElement(11)>100)
    {
        MoveForward();
        return true;
    }
    //if finish line is in the right of robot
    if(mapManager.getNearestMapElement(4)>100 ||
            mapManager.getNearestMapElement(5)>100 ||
            mapManager.getNearestMapElement(6)>100 ||
            mapManager.getNearestMapElement(12)>100 ||
            mapManager.getNearestMapElement(13)>100)
    {
        RotateRight();
        return true;
    }
    //if finish line is in the left of robot
    if(mapManager.getNearestMapElement(0)>100 ||
            mapManager.getNearestMapElement(1)>100 ||
            mapManager.getNearestMapElement(2)>100 ||
            mapManager.getNearestMapElement(7)>100 ||
            mapManager.getNearestMapElement(8)>100)
    {
        RotateLeft();
        return true;
    }
    return false;
}

void MainWindow::Rush()
{
    if(socket->isWritable())
        socketStream<<"Rush"<<endl;

}

void MainWindow::MoveFastForward()
{
    if(socket->isWritable())
        socketStream<<"FastForward"<<endl;
}

void MainWindow::MoveForward()
{
    if(socket->isWritable())
        socketStream<<"Forward"<<endl;
}

void MainWindow::Wait()
{
    if(socket->isWritable())
        socketStream<<"Wait"<<endl;
}

void MainWindow::RotateLeft()
{
    if(socket->isWritable())
        socketStream<<"RotateLeft"<<endl;
}

void MainWindow::RotateRight()
{
    if(socket->isWritable())
        socketStream<<"RotateRight"<<endl;
}

void MainWindow::client_disconnected()
{
    disconnect(socket,SIGNAL(readyRead()),this,SLOT(new_message()));
    disconnect(socket,SIGNAL(disconnected()),this,SLOT(client_disconnected()));
    socket->abort();
}

