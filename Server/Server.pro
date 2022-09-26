TEMPLATE     = app
SOURCES	     = main.cpp \
               MyServer.cpp
QT          += widgets network
HEADERS      = MyServer.h
windows:TARGET = ../Server

CONFIG += no_keywords # Python redefines some qt keywords

LIBS += -L"D:/Download/Server2factorAuth/Server" -l"python38"
INCLUDEPATH += -I "C:/Python38/include"
