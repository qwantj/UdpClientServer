#include "udpserver.h"
#include <QNetworkDatagram>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

UdpServer::UdpServer(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
}

void UdpServer::start(quint16 port)
{
    if (socket->bind(QHostAddress::Any, port)) {
        connect(socket, &QUdpSocket::readyRead, this, &UdpServer::readPendingDatagrams);
        qDebug() << "Сервер запущен на порту" << port;
    } else {
        qDebug() << "Не удалось запустить сервер на порту" << port;
    }
}

void UdpServer::readPendingDatagrams()
{
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QString message = QString::fromUtf8(datagram.data());

        qDebug() << "Сервер получил сообщение:" << message;
        qDebug() << "от" << datagram.senderAddress().toString() << ":" << datagram.senderPort();

        // Разбор сообщения и выполнение операции
        QString response;

        // Пример "небезопасного" обработчика, который может сломать сервер при делении на 0
        if (message.startsWith("UNSAFE_CALC:")) {
            QString calcStr = message.mid(12);
            QStringList parts = calcStr.split(";");
            if (parts.size() >= 3) {
                double num1 = parts[0].toDouble();
                QString operation = parts[1];
                double num2 = parts[2].toDouble();

                if (operation == "/") {
                    // Эта строка вызовет аварийное завершение программы при num2 = 0
                    double result = num1 / num2;
                    response = QString("Результат: %1").arg(result);
                } else if (operation == "+") {
                    response = QString("Результат: %1").arg(num1 + num2);
                } else if (operation == "-") {
                    response = QString("Результат: %1").arg(num1 - num2);
                } else if (operation == "*") {
                    response = QString("Результат: %1").arg(num1 * num2);
                } else {
                    response = "Неизвестная операция";
                }
            } else {
                response = "Неверный формат команды";
            }
        }
        // Пример безопасного обработчика с отловом исключений
        else if (message.startsWith("SAFE_CALC:")) {
            QString calcStr = message.mid(10);
            QStringList parts = calcStr.split(";");
            if (parts.size() >= 3) {
                double num1 = parts[0].toDouble();
                QString operation = parts[1];
                double num2 = parts[2].toDouble();

                try {
                    if (operation == "/") {
                        // Проверяем деление на ноль
                        if (qFuzzyCompare(num2, 0.0)) {
                            throw std::invalid_argument("Деление на ноль недопустимо");
                        }
                        double result = num1 / num2;
                        response = QString("Результат: %1").arg(result);
                    } else if (operation == "+") {
                        response = QString("Результат: %1").arg(num1 + num2);
                    } else if (operation == "-") {
                        response = QString("Результат: %1").arg(num1 - num2);
                    } else if (operation == "*") {
                        response = QString("Результат: %1").arg(num1 * num2);
                    } else {
                        response = "Неизвестная операция";
                    }
                } catch (const std::exception& e) {
                    response = QString("Ошибка: %1").arg(e.what());
                }
            } else {
                response = "Неверный формат команды";
            }
        }
        // Общий обработчик всех запросов с try-catch
        else {
            try {
                // Обработка обычных сообщений или других команд
                response = "Ответ сервера: " + QDateTime::currentDateTime().toString();

                // Здесь могут быть другие операции, которые потенциально могут вызвать исключения
                // ...

            } catch (const std::exception& e) {
                response = QString("Произошла ошибка: %1").arg(e.what());
            } catch (...) {
                response = "Произошла неизвестная ошибка";
            }
        }

        // Отправка ответа клиенту
        socket->writeDatagram(response.toUtf8(),
                              datagram.senderAddress(),
                              datagram.senderPort());
    }
}
