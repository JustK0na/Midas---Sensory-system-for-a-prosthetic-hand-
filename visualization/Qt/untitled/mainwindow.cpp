#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QQmlContext>
#include <QCoreApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpServer(new QTcpServer(this))
    , quickWidget(new QQuickWidget(this))
    , sampleIndex(0)
{
    ui->setupUi(this);

    // Start server
    if (!tcpServer->listen(QHostAddress::AnyIPv4, 5000)) {
        qDebug() << "Server failed to start:" << tcpServer->errorString();
    } else {
        qDebug() << "Server listening on port 5000";
        connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::handleNewConnection);
    }

    // Prepare QML quick widget and expose bridge
    quickWidget->rootContext()->setContextProperty("bridge", &bridge);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // Load QML from resource (preferred)
    quickWidget->setSource(
        QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/graph.qml")
        );
    if (quickWidget->status() != QQuickWidget::Ready) {
        qWarning() << "QML load warnings:" << quickWidget->status() << quickWidget->errors();
    }

    // Put quickWidget into central widget layout
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(quickWidget);
    setCentralWidget(central);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::handleNewConnection() {
    QTcpSocket *client = tcpServer->nextPendingConnection();
    clients.append(client);
    recvBuffers[client] = QByteArray();

    qDebug() << "New client arrived" << client->peerAddress().toString();

    connect(client, &QTcpSocket::readyRead, this, &MainWindow::readClientData);
    connect(client, &QTcpSocket::disconnected, [=]() {
        qDebug() << "Client disconnected" << client->peerAddress().toString();
        clients.removeAll(client);
        recvBuffers.remove(client);
        client->deleteLater();
    });
}

void MainWindow::readClientData() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    recvBuffers[client].append(client->readAll());

    while (true) {
        // look for newline character '\n'
        int nl = recvBuffers[client].indexOf('\n');
        if (nl == -1) break;
        QByteArray line = recvBuffers[client].left(nl);
        recvBuffers[client].remove(0, nl + 1);

        // handle optional CR at end of line ("\r\n")
        if (!line.isEmpty() && line.endsWith('\r'))
            line.chop(1);

        QString token = QString::fromUtf8(line).trimmed();
        if (token.isEmpty()) continue;

        // token format: "<id>:<value>" or just "<value>"
        int sensorId = 0; // default
        QString valueStr = token;
        int colon = token.indexOf(':');
        if (colon != -1) {
            bool okId = false;
            int parsedId = token.left(colon).toInt(&okId);
            if (okId) {
                sensorId = parsedId;
                valueStr = token.mid(colon + 1);
            }
        }

        bool ok = false;
        double value = valueStr.toDouble(&ok);
        if (!ok) {
            qDebug() << "Bad token (not a float):" << token;
            continue;
        }

        // publish to QML bridge with sensor id (use sampleIndex as X)
        bridge.publishPoint(sensorId, static_cast<qreal>(sampleIndex), static_cast<qreal>(value));
        sampleIndex++;
    }
}
