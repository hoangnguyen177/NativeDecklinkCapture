
QT       += core gui network opengl

TARGET = nativedecklinkcapture
TEMPLATE = app

# For debugging, comment out
CONFIG-=warn_on
CONFIG+=release



SOURCES += sageShader.cpp decklinkcapturedelegate.cpp include/DeckLinkAPIDispatch.cpp \
	glwidget.cpp window.cpp 
HEADERS += sageShader.h decklinkcapturedelegate.h \
	include/DeckLinkAPI.h \
	glwidget.h window.h


# For Decklink SDK
INCLUDEPATH += include /usr/include/libpng

macx {
    LIBS += 
    CONFIG -= app_bundle
}
linux-g++ {
    LIBS +=
}
linux-g++-64 {
    LIBS +=
}

LIBS += -lpthread -lm -ldl -lpng -lGLEW


target.path=.
INSTALLS += target

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Qt resources can be found in the following locations:)
message(Header files: $$[QT_INSTALL_HEADERS])
message(Libraries: $$[QT_INSTALL_LIBS])

