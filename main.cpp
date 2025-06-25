#include <QCoreApplication>
#include "udpserver.h"
#include "udpclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Создание и запуск сервера
    UdpServer server;
    server.startServer(45454);

    // Создание клиента (можно использовать для тестирования)
    UdpClient client;
    client.sendCalculation("Тестовое сообщение");

    return a.exec();
}
