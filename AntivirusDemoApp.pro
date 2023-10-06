QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Widgets/CircularDial.cpp \
    Widgets/GlowedButton.cpp \
    Widgets/SlidingStackedWidget.cpp \
    Pages/InitialPage.cpp \
    Pages/ScanProcessPage.cpp \
    ScannerMainWindow.cpp \
    main.cpp

HEADERS += \
    CommonData.h \
    Widgets/CircularDial.h \
    Widgets/CircularDial_p.h \
    Widgets/GlowedButton.h \
    Widgets/SlidingStackedWidget.h \
    Widgets/SlidingStackedWidget_p.h \
    Pages/InitialPage.h \
    Pages/InitialPage_p.h \
    Pages/ScanProcessPage.h \
    Pages/ScanProcessPage_p.h \
    ScannerMainWindow.h \
    ScannerMainWindow_p.h

CONFIG(debug, debug|release) {
    win32: DESTDIR = Debug
} else {
    win32: DESTDIR = Release
}

OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources/AntivirusDemoApp.qrc
