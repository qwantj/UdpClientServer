#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class UdpClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = nullptr);
    ~UdpClient();

    void sendCalculation(const QString &message);
    void setServerAddress(const QString &address);
    void setServerPort(quint16 port);

private:
    QUdpSocket *socket;
    QHostAddress serverAddress;
    quint16 serverPort;

signals:
    void responseReceived(const QString &response);

private slots:
    void readPendingDatagrams();
};

#endif // UDPCLIENT_H
