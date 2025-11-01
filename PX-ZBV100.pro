QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = PX-ZBV100.ico
VERSION = 1.0.0.0
TARGET = PX-ZBV100
QMAKE_TARGET_COMPANY = "深圳市鹏翔半导体技术有限公司"
QMAKE_TARGET_PRODUCT = "鹏翔测试装备上位机"
QMAKE_TARGET_DESCRIPTION = "PX-ZBV100: Created by Qt6.10.0"
QMAKE_TARGET_COPYRIGHT = "鹏翔半导体 版权所有(2025.10)"
RC_LANG = 0x0004

DESTDIR = ../../bin

QMAKE_PROJECT_DEPTH = 0

SOURCES += \
    DialogConfig.cpp \
    DialogSetting.cpp \
    LogToFile.cpp \
    gencomport.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    DialogConfig.h \
    DialogSetting.h \
    LogToFile.h \
    MainWindow.h \
    gencomport.h

FORMS += \
    DialogConfig.ui \
    DialogSetting.ui \
    MainWindow.ui

TRANSLATIONS += \
    PX-ZBV100_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
