#-------------------------------------------------
#
# Project created by QtCreator 2010-05-15T06:19:03
#
#-------------------------------------------------
VERSION = 1.0.0
QT       += core gui network

TARGET = Wallpapers
TEMPLATE = app
OBJECTS_DIR = ./builddir
MOC_DIR = ./builddir
RCC_DIR = ./builddir
DESTDIR=c:\output


SOURCES += src/main.cpp \
    src/mainview.cpp \
    src/maincontainer.cpp \
    src/button.cpp \
    src/loading.cpp \
    src/wallpaperpage.cpp \
    src/photo.cpp \
    src/wallpaperprovider.cpp \
    src/thumbnailholder.cpp \
    src/wallpaperpreviewpopup.cpp \
    src/notification.cpp \
    src/gesturebox.cpp src/system.cpp \
    src/searchbox.cpp

HEADERS  += \
    inc/mainview.h \
    inc/maincontainer.h \
    inc/button.h \
    inc/loading.h \
    inc/wallpaperpage.h \
    inc/photo.h \
    inc/wallpaperprovider.h \
    inc/thumbnailholder.h \
    inc/wallpaperpreviewpopup.h \
    inc/notification.h \
    inc/gesturebox.h \
    inc/gesturebox_p.h inc/system.h \
    inc/searchbox.h

INCLUDEPATH += inc/ ../qtflickr/

RESOURCES += images_common.qrc

win32: {
    LIBS += ../qtflickr-build/lib/libqtflickr0.a
    RESOURCES += images_symbian.qrc
}

maemo5:* {
    QT += maemo5
    LIBS += ../qtflickr-build-maemo/lib/libqtflickr.so
    RESOURCES += images_meego.qrc
}

symbian: {
        TARGET.CAPABILITY = NetworkServices WriteDeviceData
        TARGET.EPOCHEAPSIZE = 0x20000 0x2000000

        TARGET.UID3 = 0x200328A
        
        LIBS += -lqtflickr
        LIBS += -lcone -leikcore -lavkon -laknswallpaperutils -ltouchfeedback
	
        qtflickr.sources = qtflickr.dll
        qtflickr.path = \sys\bin
        DEPLOYMENT += qtflickr
		
        ICON=symbian/icon.svg
        RESOURCES += images_symbian.qrc
}
