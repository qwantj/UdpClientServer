#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);
    ~UdpServer();

    bool startServer(quint16 port);
    void stopServer();

private:
    QUdpSocket *socket;

private slots:
    void readPendingDatagrams();
};

#endif // UDPSERVER_H
