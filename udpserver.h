#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);
    void start(quint16 port);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket *socket;
};

#endif // UDPSERVER_H
