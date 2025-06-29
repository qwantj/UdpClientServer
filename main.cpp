#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include <QTimer>
#include <iostream>
#include <string>
#include "udpserver.h"
#include "udpclient.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

void messageHandler(QtMsgType type, [[maybe_unused]] const QMessageLogContext &context, const QString &msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString logMsg;

    switch (type) {
    case QtDebugMsg:
        logMsg = QString("[%1] DEBUG: %2").arg(timestamp).arg(msg);
        break;
    case QtInfoMsg:
        logMsg = QString("[%1] INFO: %2").arg(timestamp).arg(msg);
        break;
    case QtWarningMsg:
        logMsg = QString("[%1] WARNING: %2").arg(timestamp).arg(msg);
        break;
    case QtCriticalMsg:
        logMsg = QString("[%1] CRITICAL: %2").arg(timestamp).arg(msg);
        break;
    case QtFatalMsg:
        logMsg = QString("[%1] FATAL: %2").arg(timestamp).arg(msg);
        break;
    }

#ifdef Q_OS_WIN
    // Для Windows используем стандартный поток вывода
    std::wcout << logMsg.toStdWString() << std::endl;
#else
    // Для других платформ используем UTF-8
    fprintf(stderr, "%s\n", logMsg.toUtf8().constData());
    fflush(stderr);
#endif
}

void printUsage() {
    qDebug() << "Использование:";
    qDebug() << "  UdpClientServer server [порт]          - Запустить сервер";
    qDebug() << "  UdpClientServer client [сообщение]     - Запустить клиент";
}

// Функция для чтения ввода пользователя
void processInput(UdpClient *client) {
    std::string input;
    qDebug() << "Введите выражение (или 'exit' для выхода): ";
    std::getline(std::cin, input);

    if (input == "exit") {
        qDebug() << "Завершение работы клиента...";
        QCoreApplication::quit();
        return;
    }

    if (!input.empty()) {
        QString message = QString::fromStdString(input);
        qDebug() << "Отправка сообщения:" << message;
        client->sendCalculation(message);
    }

    // Планируем следующее чтение после обработки текущего запроса
    QTimer::singleShot(100, [=]() { processInput(client); });  // Используем capture by value
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // Настройка консоли Windows для правильного отображения кириллицы
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // Используем стандартную локаль
    std::locale::global(std::locale(""));
#endif

    qInstallMessageHandler(messageHandler);
    QCoreApplication a(argc, argv);

    qDebug() << "Программа запущена";

    if (argc < 2) {
        printUsage();
        return 1;
    }

    QString mode = argv[1];

    if (mode == "server") {
        quint16 port = 45454;
        if (argc > 2) {
            port = QString(argv[2]).toUInt();
        }

        qDebug() << "Запуск UDP сервера...";
        UdpServer *server = new UdpServer(&a);
        if (server->startServer(port)) {
            qDebug() << "Сервер успешно запущен на порту" << port;
            qDebug() << "Ожидание подключений...";
        } else {
            qDebug() << "Не удалось запустить сервер на порту" << port;
            return 1;
        }
    }
    else if (mode == "client") {
        qDebug() << "Запуск UDP клиента...";
        UdpClient *client = new UdpClient(&a);

        // Подключение к событию получения ответа
        QObject::connect(client, &UdpClient::responseReceived,
                         [](const QString &response){
                             qDebug() << "Получен ответ:" << response;
                             std::cout << std::endl;  // Дополнительный перевод строки для удобства
                         });

        // Если передано выражение в аргументах, отправляем его сразу
        if (argc > 2) {
            QString initialMessage = QString(argv[2]);
            qDebug() << "Отправка начального выражения:" << initialMessage;
            client->sendCalculation(initialMessage);
        }

        // Запускаем интерактивный режим
        qDebug() << "Включен интерактивный режим. Вводите выражения или 'exit' для выхода";
        QTimer::singleShot(500, [=]() { processInput(client); });  // Используем capture by value
    }
    else {
        printUsage();
        return 1;
    }

    return a.exec();
}
