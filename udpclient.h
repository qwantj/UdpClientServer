#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>  // Добавляем заголовок для QUdpSocket
#include <QHostAddress> // Добавляем заголовок для QHostAddress

class UdpClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = nullptr);
    ~UdpClient();

    // Исправляем опечатку в имени метода (было sendCakulation)
    void sendCalculation(const QString &message);

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
