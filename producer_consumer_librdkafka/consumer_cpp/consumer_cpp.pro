TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    cconsumer.cpp

unix:!macx: LIBS += -lrdkafka++

HEADERS += \
    cconsumer.h
