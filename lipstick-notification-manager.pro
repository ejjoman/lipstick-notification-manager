# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TEMPLATE = app
TARGET = lipstick-notification-manager
CONFIG += console
QT += core dbus
QT -= gui

LIBS = -llipstick-qt5
#INCLUDEPATH += /usr/include/lipstick-qt5
INCLUDEPATH += $$[QT_HOST_PREFIX]/include/lipstick-qt5

target.path = /usr/bin
INSTALLS += target

SOURCES += \
    src/main.cpp \
    src/application.cpp

OTHER_FILES += rpm/lipstick-notification-manager.changes.in \
    rpm/lipstick-notification-manager.spec \
    rpm/lipstick-notification-manager.yaml

HEADERS += \
    src/application.h
