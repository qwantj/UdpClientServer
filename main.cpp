#include <QCoreApplication>
#include <QCommandLineParser>
#include <QHostAddress>
#include <QTimer>
#include <QDebug>
#include "udpserver.h"
#include "udpclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("UDP Client-Server Example with Error Handling");
    parser.addHelpOption();

    QCommandLineOption serverOption({"s", "server"}, "Run as server");
    parser.addOption(serverOption);

    QCommandLineOption clientOption({"c", "client"}, "Run as client");
    parser.addOption(clientOption);

    QCommandLineOption portOption({"p", "port"}, "Port number", "port", "45454");
    parser.addOption(portOption);

    QCommandLineOption addressOption({"a", "address"}, "Server address (for client mode)", "address", "127.0.0.1");
    parser.addOption(addressOption);

    QCommandLineOption testOption({"t", "test"}, "Run arithmetic test (for client mode)", "test", "safe");
    parser.addOption(testOption);

    parser.process(a);

    bool isServer = parser.isSet(serverOption);
    bool isClient = parser.isSet(clientOption);
    quint16 port = parser.value(portOption).toUShort();
    QHostAddress address(parser.value(addressOption));
    QString testMode = parser.value(testOption);

    if (isServer) {
        qDebug() << "Запуск в режиме сервера";
        UdpServer server;
        server.start(port);
        return a.exec();
    }
    else if (isClient) {
        qDebug() << "Запуск в режиме клиента";
        UdpClient client;

        // Обычное приветственное сообщение
        client.sendMessage("Привет от клиента!", address, port);

        // Тестирование арифметических операций
        QTimer::singleShot(1000, [&client, &address, port, testMode]() {
            if (testMode == "unsafe") {
                qDebug() << "Тестирование небезопасного режима (деление на 0)...";
                client.sendCalculation(10.0, "/", 0.0, false, address, port);
            } else {
                qDebug() << "Тестирование безопасного режима (деление на 0)...";
                client.sendCalculation(10.0, "/", 0.0, true, address, port);

                // Добавляем и другие безопасные операции для проверки
                QTimer::singleShot(1000, [&client, &address, port]() {
                    client.sendCalculation(5.0, "+", 3.0, true, address, port);
                });

                QTimer::singleShot(2000, [&client, &address, port]() {
                    client.sendCalculation(8.0, "*", 4.0, true, address, port);
                });
            }
        });

        return a.exec();
    }
    else {
        qDebug() << "Укажите режим работы: --server или --client";
        return 1;
    }
}
