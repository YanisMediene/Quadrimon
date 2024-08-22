QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG   += console
CONFIG   -= app_bundle
QT += widgets

SOURCES += \
    capsuleterrain.cpp \
    combat.cpp \
    joueur.cpp \
    main.cpp \
    mainwindow.cpp \
    myglwidget.cpp \
    quadrimon.cpp \
    terrain.cpp

HEADERS += \
    capsuleterrain.h \
    combat.h \
    joueur.h \
    mainwindow.h \
    myglwidget.h \
    quadrimon.h \
    terrain.h

FORMS += \
    mainwindow.ui

equals(QT_MAJOR_VERSION, 5) {
        QT += opengl widgets
} else:equals(QT_MAJOR_VERSION, 6) {
        QT += openglwidgets
}

# ajout des libs au linker
win32 {
    win32-msvc* {
        LIBS     += opengl32.lib glu32.lib
    } else {
        LIBS     += -lopengl32 -lglu32
    }
} else:macx {
        LIBS     += -framework OpenGL
} else {
        LIBS     += -lGL -lGLU
}

INCLUDEPATH += /usr/local/include/opencv4

LIBS += -L/usr/local/lib \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_imgcodecs \
    -lopencv_videoio \
    -lopencv_features2d

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Ressource.qrc
