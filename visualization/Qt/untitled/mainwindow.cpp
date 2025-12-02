#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);

    if(!tcpServer->listen(QHostAddress::AnyIPv4, 5000)){
        qDebug() << "Server failed to start\n" << tcpServer->errorString();
    }
    else{
        qDebug() << "Server listening to port 5000.";
        connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::handleNewConection);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleNewConection(){
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    clients.append(clientSocket);
    qDebug() << "New client arrived" << clientSocket->peerAddress().toString();

    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::readClientData);
    connect(clientSocket, &QTcpSocket::disconnected, [=](){
        qDebug() << "Client dissconeted" << clientSocket->peerAddress().toString();
        clients.removeAll(clientSocket);
        clientSocket->deleteLater();
    });
}

void MainWindow::readClientData(){
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if(!client)
        return;
    QByteArray data = client->readAll();
    QString message = QString::fromUtf8(data);
    qDebug() << "Recived from" << client->peerAddress().toString() << ":" << message;
}
