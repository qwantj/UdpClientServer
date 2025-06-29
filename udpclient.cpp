#include "udpclient.h"
#include <QDebug>

UdpClient::UdpClient(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);

    // Настройка клиента для прослушивания ответов
    connect(socket, &QUdpSocket::readyRead,
            this, &UdpClient::readPendingDatagrams);

    // Настройка сервера по умолчанию
    serverAddress = QHostAddress::LocalHost;
    serverPort = 45454;

    qDebug() << "UDP клиент создан, целевой адрес:" << serverAddress.toString()
             << ", порт:" << serverPort;
}

UdpClient::~UdpClient()
{
    if (socket) {
        socket->close();
    }
}

void UdpClient::setServerAddress(const QString &address)
{
    serverAddress = QHostAddress(address);
    qDebug() << "Адрес сервера изменен на:" << serverAddress.toString();
}

void UdpClient::setServerPort(quint16 port)
{
    serverPort = port;
    qDebug() << "Порт сервера изменен на:" << port;
}

void UdpClient::sendCalculation(const QString &message)
{
    QByteArray datagram = message.toUtf8();
    qint64 bytesSent = socket->writeDatagram(datagram, serverAddress, serverPort);

    if (bytesSent == -1) {
        qDebug() << "Ошибка отправки: " << socket->errorString();
    } else {
        qDebug() << "Успешно отправлено байт:" << bytesSent;
        qDebug() << "Отправлено на сервер (" << serverAddress.toString() << ":" << serverPort << "): " << message;
    }
}

void UdpClient::readPendingDatagrams()
{
    qDebug() << "Клиент получает ответ...";

    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString response = QString::fromUtf8(datagram);
        qDebug() << "Получено от " << sender.toString() << ":" << senderPort << ", ответ:" << response;

        emit responseReceived(response);
    }
}
