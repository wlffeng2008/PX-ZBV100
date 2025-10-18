QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
