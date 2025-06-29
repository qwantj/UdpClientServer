#include "udpserver.h"
#include <QDebug>
#include <QJSEngine>

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
        qDebug() << "UDP сокет создан";
    }

    if (!socket->bind(QHostAddress::Any, port)) {
        qDebug() << "Ошибка привязки сервера к порту:" << socket->errorString();
        return false;
    }

    connect(socket, &QUdpSocket::readyRead,
            this, &UdpServer::readPendingDatagrams);

    // Вывод информации о сервере
    qDebug() << "Сервер привязан к порту:" << socket->localPort();
    QHostAddress localAddress = QHostAddress::LocalHost;
    qDebug() << "Локальный адрес (localhost):" << localAddress.toString();
    qDebug() << "Сервер готов принимать соединения";

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
    qDebug() << "Чтение поступивших данных...";

    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString message = QString::fromUtf8(datagram);

        // Определяем, является ли отправитель локальным хостом
        if (sender == QHostAddress::LocalHost) {
            qDebug() << "Получено от localhost (127.0.0.1), порт:" << senderPort << ", сообщение:" << message;
        } else {
            qDebug() << "Получено от удаленного адреса:" << sender.toString()
                     << ", порт:" << senderPort << ", сообщение:" << message;
        }

        qDebug() << "Обработка выражения: " << message;

        // Обработка и вычисление выражения
        QString response;
        try {
            // Используем QJSEngine для безопасного вычисления выражения
            QJSEngine engine;
            QJSValue result = engine.evaluate(message);

            if (result.isError()) {
                response = "Ошибка: " + result.toString();
            } else {
                response = result.toString();
            }
        } catch (...) {
            response = "Ошибка при вычислении";
        }

        qDebug() << "Отправка ответа: " << response;
        socket->writeDatagram(response.toUtf8(), sender, senderPort);
    }
}
