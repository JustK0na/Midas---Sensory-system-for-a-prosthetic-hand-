#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QQuickWidget>
#include <QHash>

#include "graphbridge.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void handleNewConnection();
    void readClientData();

private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clients;
    QHash<QTcpSocket*, QByteArray> recvBuffers;

    // QML + Graph bridge
    QQuickWidget *quickWidget;
    GraphBridge bridge;

    qint64 sampleIndex;
};

#endif // MAINWINDOW_H
