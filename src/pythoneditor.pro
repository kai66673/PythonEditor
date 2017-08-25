QT       += widgets
TARGET = pythoneditor
TEMPLATE = lib

DEFINES += PYTHONEDITOR_LIBRARY

unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS += \
    pythoneditor_global.h \
    pythoneditor.h \
    pythonscanner.h \
    pythonhighlighter.h \
    pythonformattoken.h

SOURCES += \
    pythoneditor.cpp \
    pythonscanner.cpp \
    pythonhighlighter.cpp
