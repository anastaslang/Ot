QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/include

SOURCES += \
    $$PWD/src/loginwidget.cpp \
    $$PWD/src/main.cpp \
    $$PWD/src/clientmainwindow.cpp \
    $$PWD/src/registrationwidget.cpp

HEADERS += \
    $$PWD/include/clientmainwindow.h \
    $$PWD/include/loginwidget.h \
    $$PWD/include/registrationwidget.h

FORMS += \
    $$PWD/gui/clientmainwindow.ui \
    $$PWD/gui/loginwidget.ui \
    $$PWD/gui/registrationwidget.ui

RESOURCES += \
    $$PWD/style/style1.qss

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
