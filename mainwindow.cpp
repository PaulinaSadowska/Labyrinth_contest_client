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
            priorityLeft = false;
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

    if(onePlaceCounter>4)
    {
        if(mapManager.ForwardPoints[0]>0)
        {
            MoveForward();
            return true;
        }
    }

    if(mapManager.LeftPoints[0]==0 && mapManager.ForwardPoints[0]<0 && mapManager.RightPoints[0]==0)
    {
        RotateRight();
        return true;
    }

    if((mapManager.LeftPoints[0]==mapManager.RightPoints[0] && (mapManager.RightPoints[1]==0 || mapManager.LeftPoints[1]==0) && mapManager.ForwardPoints[0]<0) ||
            (mapManager.LeftPoints[0]==mapManager.ForwardPoints[0] && mapManager.LeftPoints[1]==0) ||
            (mapManager.RightPoints[0]==mapManager.ForwardPoints[0] && mapManager.RightPoints[1]==0))
    {
            Wait();
            return true;
    }

    //DEAD END
    if(mapManager.LeftPoints[0]<0 && mapManager.ForwardPoints[0]<0 && mapManager.RightPoints[0]<0)
    {
        RotateRight();
        return true;
    }

    char dir;
    dir = ChooseDirection();

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
        else if(mapManager.ForwardPoints[0]==mapManager.ForwardPoints[1] &&
                mapManager.ForwardPoints[0]<49)
            Rush();
        else
            MoveFastForward();
        return true;
    }
    default :
        return false;
    }

    return false;


}

char MainWindow::ChooseDirection()
{
    int max = 0;
    char dir;

    if(priorityLeft)
    {
        if(mapManager.LeftPoints[0]>(max-1) && (mapManager.LeftPoints[0]<74 ||
            mapManager.LeftPoints[1]==500 || mapManager.LeftPoints[2]==500 ||
                                            mapManager.ForwardPoints[0]<0) )
        {
                max = mapManager.LeftPoints[0];
                dir = 'L';
        }
        if(mapManager.ForwardPoints[0]>max)
        {
            max = mapManager.ForwardPoints[0];
            dir = 'F';
        }
        if(mapManager.RightPoints[0]>(max+6) && mapManager.LeftPoints[0]<74)
        {
            max = mapManager.RightPoints[0];
            dir = 'R';
        }
    }
    else
    {
        if(mapManager.RightPoints[0]>(max-1) && (mapManager.RightPoints[0]<74 ||
           mapManager.RightPoints[1]==500 || mapManager.RightPoints[2]==500 ||
                                                 mapManager.ForwardPoints[0]<0) )
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
    }
    if(mapManager.ForwardPoints[0]==mapManager.RightPoints[0] && mapManager.ForwardPoints[0]%2==0)
    {
        max = mapManager.ForwardPoints[0];
        dir = 'F';
    }
    return dir;
}

void MainWindow::CheckPriority()
{
    for(int i=0; i<28; i++)
    {
        if(i%7<3)
        {
            if(mapManager.getNearestMapElement(i)>100)
            {
                setPriorityLeft();
            }
        }
    }
}

bool MainWindow::LookForFinishLine()
{
    CheckPriority();

    //if finish line in front of robot
    if(     mapManager.getNearestMapElement(23)>100 ||
            mapManager.getNearestMapElement(24)>100 ||
            mapManager.getNearestMapElement(25)>100)
    {
        if(mapManager.ForwardPoints[0]>0)
        {
            if(mapManager.ForwardPoints[1]>0)
            {
                if(mapManager.ForwardPoints[2]>0)
                {
                    Rush();
                    return true;
                }
                MoveFastForward();
                return true;
            }
            MoveForward();
            return true;
        }
    }
    //if finish line is in the right of robot
    if(     mapManager.getNearestMapElement(4)>100 ||
            mapManager.getNearestMapElement(5)>100 ||
            mapManager.getNearestMapElement(6)>100)
    {
        if(mapManager.RightPoints[0]>0)
        {
            RotateRight();
            return true;
        }

    }
    //if finish line is in the left of robot
    if(mapManager.getNearestMapElement(0)>100 ||
            mapManager.getNearestMapElement(1)>100 ||
            mapManager.getNearestMapElement(2)>100 )
    {
        if(mapManager.LeftPoints[0]>0)
        {
            RotateLeft();
            return true;
        }
    }
    return false;
}
void MainWindow::setPriorityLeft()
{
    priorityLeft = true;
}

void MainWindow::resetPriorityLeft()
{
    priorityLeft = false;
}

void MainWindow::Rush()
{
    onePlaceCounter=0;
    if(socket->isWritable())
        socketStream<<"Rush"<<endl;

}

void MainWindow::MoveFastForward()
{
    onePlaceCounter=0;
    if(socket->isWritable())
        socketStream<<"FastForward"<<endl;
}

void MainWindow::MoveForward()
{
    onePlaceCounter=0;
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

