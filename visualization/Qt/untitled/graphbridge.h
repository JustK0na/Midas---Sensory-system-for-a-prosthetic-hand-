#ifndef GRAPHBRIDGE_H
#define GRAPHBRIDGE_H

#include <QObject>

class GraphBridge : public QObject {
    Q_OBJECT
public:
    explicit GraphBridge(QObject *parent = nullptr) : QObject(parent) {}

    // publish a new point for a given sensor id
    Q_INVOKABLE void publishPoint(int sensorId, qreal x, qreal y) { emit newPoint(sensorId, x, y); }

signals:
    void newPoint(int sensorId, qreal x, qreal y);
};

#endif // GRAPHBRIDGE_H
