QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cssstyle.cpp \
    data.cpp \
    main.cpp \
    mainwindow.cpp \
    scene.cpp \
    slide.cpp \
    verseListView/verseitem.cpp \
    verseListView/verselistview.cpp

HEADERS += \
    cssstyle.h \
    data.h \
    mainwindow.h \
    scene.h \
    slide.h \
    verseListView/verseitem.h \
    verseListView/verselistview.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
