#include "udpclient.h"
#include <QNetworkDatagram>
#include <QDebug>

UdpClient::UdpClient(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
    socket->bind();  // Привязка к произвольному порту
    connect(socket, &QUdpSocket::readyRead, this, &UdpClient::readPendingDatagrams);
}

void UdpClient::sendMessage(const QString &message, const QHostAddress &address, quint16 port)
{
    qDebug() << "Клиент отправляет сообщение:" << message;
    QByteArray datagram = message.toUtf8();
    socket->writeDatagram(datagram, address, port);
}

// Метод для отправки арифметических операций
void UdpClient::sendCalculation(double num1, const QString &operation, double num2,
                                bool safe, const QHostAddress &address, quint16 port)
{
    QString prefix = safe ? "SAFE_CALC:" : "UNSAFE_CALC:";
    QString message = QString("%1%2;%3;%4").arg(prefix).arg(num1).arg(operation).arg(num2);

    sendMessage(message, address, port);
}

void UdpClient::readPendingDatagrams()
{
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QString message = QString::fromUtf8(datagram.data());

        qDebug() << "Клиент получил ответ:" << message;
        qDebug() << "от" << datagram.senderAddress().toString() << ":" << datagram.senderPort();
    }
}
