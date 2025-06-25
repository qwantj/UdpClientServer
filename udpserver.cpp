#include "udpserver.h"
#include <QDebug>

UdpServer::UdpServer(QObject *parent) : QObject(parent), socket(nullptr)
{
}

UdpServer::~UdpServer()
{
    stopServer();
}

bool UdpServer::startServer(quint16 port)
{
    if (socket == nullptr) {
        socket = new QUdpSocket(this);
    }

    if (!socket->bind(QHostAddress::Any, port)) {
        qDebug() << "Ошибка привязки сервера к порту:" << socket->errorString();
        return false;
    }

    connect(socket, &QUdpSocket::readyRead,
            this, &UdpServer::readPendingDatagrams);

    qDebug() << "Сервер запущен на порту:" << port;
    return true;
}

void UdpServer::stopServer()
{
    if (socket) {
        socket->close();
        delete socket;
        socket = nullptr;
        qDebug() << "Сервер остановлен";
    }
}

void UdpServer::readPendingDatagrams()
{
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString message = QString::fromUtf8(datagram);
        qDebug() << "Получено от клиента:" << message;

        // Пример обработки данных и отправки ответа
        QString response = "Сервер получил: " + message;
        socket->writeDatagram(response.toUtf8(), sender, senderPort);
    }
}
