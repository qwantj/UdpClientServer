QT += core network qml
QT -= gui

CONFIG += console
CONFIG += c++17
CONFIG -= app_bundle

TARGET = UdpClientServer
TEMPLATE = app

SOURCES += \
    main.cpp \
    udpclient.cpp \
    udpserver.cpp

HEADERS += \
    udpclient.h \
    udpserver.h
