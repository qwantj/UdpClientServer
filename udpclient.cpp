#include "udpclient.h"
#include <QDebug>

UdpClient::UdpClient(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);

    // Настройка клиента для прослушивания ответов
    connect(socket, &QUdpSocket::readyRead,
            this, &UdpClient::readPendingDatagrams);

    // Настройка сервера по умолчанию (можно изменить)
    serverAddress = QHostAddress::LocalHost;
    serverPort = 45454;
}

UdpClient::~UdpClient()
{
    // Qt автоматически удалит socket, так как он имеет родителя (this)
}

void UdpClient::sendCalculation(const QString &message)
{
    QByteArray datagram = message.toUtf8();
    socket->writeDatagram(datagram, serverAddress, serverPort);
    qDebug() << "Отправлено на сервер:" << message;
}

void UdpClient::readPendingDatagrams()
{
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString response = QString::fromUtf8(datagram);
        qDebug() << "Получено от сервера:" << response;
        emit responseReceived(response);
    }
}
