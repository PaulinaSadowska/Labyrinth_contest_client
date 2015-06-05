#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),socket(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket=new QTcpSocket(this);

    for(int i=0;i<7;i++)
        for(int j=0;j<4;j++)
            ui->robotVisionTable->setItem(j,i,new QTableWidgetItem("?"));
    for(int i=0;i<7;i++)
        ui->robotVisionTable->setColumnWidth(i,30);
    for(int i=0;i<4;i++)
        ui->robotVisionTable->setRowHeight(i,30);

    //nextMove = Whatever;
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
        else if(localMap[0]=='X')
        {
            robotManager.Init();
        }
        //update global map
        mapManager.UpdateMap(robotManager, localMap);

        /// the easiest algorithm:
        /// go ahead and when yu can't - turn right
        ///

        /*if(nextMove==Whatever || nextMove==Forward)
        {
            if(localMap[14]=='.' || localMap[14]=='E')
                MoveForward();
            if(localMap[15]=='.')
                nextMove=Right;
            else if(localMap[14]!='.' && localMap[14]!='E')
            {
                RotateLeft();
                nextMove=Forward;
            }


        }
        else if(nextMove==Right)
        {
            RotateRight();
            nextMove=Forward;
        }*/
    }

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

