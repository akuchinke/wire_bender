#-------------------------------------------------
#
# Project created by QtCreator 2016-06-20T20:08:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wire_bender
TEMPLATE = app


SOURCES +=\
    src/util/abstractexercise.cpp \
    src/util/abstractpainter.cpp \
    src/util/camera.cpp \
    src/util/fileassociatedasset.cpp \
    src/util/fileassociatedshader.cpp \
    src/util/glviewer.cpp \
    src/util/icosahedron.cpp \
    src/util/objio.cpp \
    src/util/polygonaldrawable.cpp \
    src/util/wire.cpp \
    src/util/wirecreator.cpp \
    src/viewer.cpp




HEADERS  += \
    src/util/abstractexercise.h \
    src/util/abstractpainter.h \
    src/util/cachedvalue.h \
    src/util/cachedvalue.hpp \
    src/util/camera.h \
    src/util/fileassociatedasset.h \
    src/util/fileassociatedshader.h \
    src/util/glviewer.h \
    src/util/icosahedron.h \
    src/util/objio.h \
    src/util/openglfunctions.h \
    src/util/paintermode.h \
    src/util/polygonaldrawable.h \
    src/util/wire.h \
    src/util/wirecreator.h

release: DESTDIR = release
debug:   DESTDIR = debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
