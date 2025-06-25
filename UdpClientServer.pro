# Добавление модуля network в проект
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Остальная часть вашего .pro файла
TARGET = UdpClientServer
TEMPLATE = app

SOURCES += \
    main.cpp \
    udpclient.cpp \
    udpserver.cpp

HEADERS += \
    udpclient.h \
    udpserver.h
