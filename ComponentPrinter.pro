TEMPLATE = lib
TARGET = Printer
QT += qml quick printsupport
CONFIG += qt plugin

qtHaveModule(printsupport) {
    QT += printsupport
}
!qtHaveModule(printsupport) {
    DEFINES += QT_NO_PRINTER
}

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.foxmoxie.Printer

# Input
SOURCES += \
    componentprinter_plugin.cpp \
    printer.cpp

HEADERS += \
    componentprinter_plugin.h \
    printer.h

DISTFILES = qmldir \
    LICENSE \
    README.md \
    examples/imagecomponents.qml \
    examples/imagefile.qml \
    examples/multipage.qml \
    examples/singlepage.qml

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix|win32 {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

