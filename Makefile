#############################################################################
# Makefile for building: nativedecklinkcapture
# Generated by qmake (2.01a) (Qt 4.8.5) on: Tue Aug 12 11:15:17 2014
# Project:  decklinkcapture.pro
# Template: app
# Command: /usr/bin/qmake -o Makefile decklinkcapture.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/lib64/qt/mkspecs/linux-g++ -I. -I/usr/lib64/qt/include/QtCore -I/usr/lib64/qt/include/QtNetwork -I/usr/lib64/qt/include/QtGui -I/usr/lib64/qt/include/QtOpenGL -I/usr/lib64/qt/include -Iinclude -I/usr/include -I.
LINK          = g++
LFLAGS        = -Wl,-O1 -Wl,-rpath,/usr/lib64/qt/lib
LIBS          = $(SUBLIBS)  -L/usr/lib64/qt/lib -L/usr/X11R6/lib -lm -ldl -lQtOpenGL -L/usr/lib64/qt/lib -L/usr/X11R6/lib -lQtGui -lQtNetwork -lQtCore -lGL -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = decklinkcapturedelegate.cpp \
		include/DeckLinkAPIDispatch.cpp \
		glwidget.cpp \
		window.cpp moc_glwidget.cpp \
		moc_window.cpp
OBJECTS       = decklinkcapturedelegate.o \
		DeckLinkAPIDispatch.o \
		glwidget.o \
		window.o \
		moc_glwidget.o \
		moc_window.o
DIST          = /usr/lib64/qt/mkspecs/common/unix.conf \
		/usr/lib64/qt/mkspecs/common/linux.conf \
		/usr/lib64/qt/mkspecs/common/gcc-base.conf \
		/usr/lib64/qt/mkspecs/common/gcc-base-unix.conf \
		/usr/lib64/qt/mkspecs/common/g++-base.conf \
		/usr/lib64/qt/mkspecs/common/g++-unix.conf \
		/usr/lib64/qt/mkspecs/qconfig.pri \
		/usr/lib64/qt/mkspecs/modules/qt_phonon.pri \
		/usr/lib64/qt/mkspecs/modules/qt_webkit_version.pri \
		/usr/lib64/qt/mkspecs/features/qt_functions.prf \
		/usr/lib64/qt/mkspecs/features/qt_config.prf \
		/usr/lib64/qt/mkspecs/features/exclusive_builds.prf \
		/usr/lib64/qt/mkspecs/features/default_pre.prf \
		/usr/lib64/qt/mkspecs/features/release.prf \
		/usr/lib64/qt/mkspecs/features/default_post.prf \
		/usr/lib64/qt/mkspecs/features/shared.prf \
		/usr/lib64/qt/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/lib64/qt/mkspecs/features/qt.prf \
		/usr/lib64/qt/mkspecs/features/unix/opengl.prf \
		/usr/lib64/qt/mkspecs/features/unix/thread.prf \
		/usr/lib64/qt/mkspecs/features/moc.prf \
		/usr/lib64/qt/mkspecs/features/resources.prf \
		/usr/lib64/qt/mkspecs/features/uic.prf \
		/usr/lib64/qt/mkspecs/features/yacc.prf \
		/usr/lib64/qt/mkspecs/features/lex.prf \
		/usr/lib64/qt/mkspecs/features/include_source_dir.prf \
		decklinkcapture.pro
QMAKE_TARGET  = nativedecklinkcapture
DESTDIR       = 
TARGET        = nativedecklinkcapture

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: decklinkcapture.pro  /usr/lib64/qt/mkspecs/linux-g++/qmake.conf /usr/lib64/qt/mkspecs/common/unix.conf \
		/usr/lib64/qt/mkspecs/common/linux.conf \
		/usr/lib64/qt/mkspecs/common/gcc-base.conf \
		/usr/lib64/qt/mkspecs/common/gcc-base-unix.conf \
		/usr/lib64/qt/mkspecs/common/g++-base.conf \
		/usr/lib64/qt/mkspecs/common/g++-unix.conf \
		/usr/lib64/qt/mkspecs/qconfig.pri \
		/usr/lib64/qt/mkspecs/modules/qt_phonon.pri \
		/usr/lib64/qt/mkspecs/modules/qt_webkit_version.pri \
		/usr/lib64/qt/mkspecs/features/qt_functions.prf \
		/usr/lib64/qt/mkspecs/features/qt_config.prf \
		/usr/lib64/qt/mkspecs/features/exclusive_builds.prf \
		/usr/lib64/qt/mkspecs/features/default_pre.prf \
		/usr/lib64/qt/mkspecs/features/release.prf \
		/usr/lib64/qt/mkspecs/features/default_post.prf \
		/usr/lib64/qt/mkspecs/features/shared.prf \
		/usr/lib64/qt/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/lib64/qt/mkspecs/features/qt.prf \
		/usr/lib64/qt/mkspecs/features/unix/opengl.prf \
		/usr/lib64/qt/mkspecs/features/unix/thread.prf \
		/usr/lib64/qt/mkspecs/features/moc.prf \
		/usr/lib64/qt/mkspecs/features/resources.prf \
		/usr/lib64/qt/mkspecs/features/uic.prf \
		/usr/lib64/qt/mkspecs/features/yacc.prf \
		/usr/lib64/qt/mkspecs/features/lex.prf \
		/usr/lib64/qt/mkspecs/features/include_source_dir.prf \
		/usr/lib64/qt/lib/libQtOpenGL.prl \
		/usr/lib64/qt/lib/libQtGui.prl \
		/usr/lib64/qt/lib/libQtCore.prl \
		/usr/lib64/qt/lib/libQtNetwork.prl
	$(QMAKE) -o Makefile decklinkcapture.pro
/usr/lib64/qt/mkspecs/common/unix.conf:
/usr/lib64/qt/mkspecs/common/linux.conf:
/usr/lib64/qt/mkspecs/common/gcc-base.conf:
/usr/lib64/qt/mkspecs/common/gcc-base-unix.conf:
/usr/lib64/qt/mkspecs/common/g++-base.conf:
/usr/lib64/qt/mkspecs/common/g++-unix.conf:
/usr/lib64/qt/mkspecs/qconfig.pri:
/usr/lib64/qt/mkspecs/modules/qt_phonon.pri:
/usr/lib64/qt/mkspecs/modules/qt_webkit_version.pri:
/usr/lib64/qt/mkspecs/features/qt_functions.prf:
/usr/lib64/qt/mkspecs/features/qt_config.prf:
/usr/lib64/qt/mkspecs/features/exclusive_builds.prf:
/usr/lib64/qt/mkspecs/features/default_pre.prf:
/usr/lib64/qt/mkspecs/features/release.prf:
/usr/lib64/qt/mkspecs/features/default_post.prf:
/usr/lib64/qt/mkspecs/features/shared.prf:
/usr/lib64/qt/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/lib64/qt/mkspecs/features/qt.prf:
/usr/lib64/qt/mkspecs/features/unix/opengl.prf:
/usr/lib64/qt/mkspecs/features/unix/thread.prf:
/usr/lib64/qt/mkspecs/features/moc.prf:
/usr/lib64/qt/mkspecs/features/resources.prf:
/usr/lib64/qt/mkspecs/features/uic.prf:
/usr/lib64/qt/mkspecs/features/yacc.prf:
/usr/lib64/qt/mkspecs/features/lex.prf:
/usr/lib64/qt/mkspecs/features/include_source_dir.prf:
/usr/lib64/qt/lib/libQtOpenGL.prl:
/usr/lib64/qt/lib/libQtGui.prl:
/usr/lib64/qt/lib/libQtCore.prl:
/usr/lib64/qt/lib/libQtNetwork.prl:
qmake:  FORCE
	@$(QMAKE) -o Makefile decklinkcapture.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/nativedecklinkcapture1.0.0 || $(MKDIR) .tmp/nativedecklinkcapture1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/nativedecklinkcapture1.0.0/ && $(COPY_FILE) --parents decklinkcapturedelegate.h include/DeckLinkAPI.h glwidget.h window.h .tmp/nativedecklinkcapture1.0.0/ && $(COPY_FILE) --parents decklinkcapturedelegate.cpp include/DeckLinkAPIDispatch.cpp glwidget.cpp window.cpp .tmp/nativedecklinkcapture1.0.0/ && (cd `dirname .tmp/nativedecklinkcapture1.0.0` && $(TAR) nativedecklinkcapture1.0.0.tar nativedecklinkcapture1.0.0 && $(COMPRESS) nativedecklinkcapture1.0.0.tar) && $(MOVE) `dirname .tmp/nativedecklinkcapture1.0.0`/nativedecklinkcapture1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/nativedecklinkcapture1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_glwidget.cpp moc_window.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_glwidget.cpp moc_window.cpp
moc_glwidget.cpp: glwidget.h
	/usr/lib64/qt/bin/moc $(DEFINES) $(INCPATH) glwidget.h -o moc_glwidget.cpp

moc_window.cpp: decklinkcapturedelegate.h \
		glwidget.h \
		window.h
	/usr/lib64/qt/bin/moc $(DEFINES) $(INCPATH) window.h -o moc_window.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

decklinkcapturedelegate.o: decklinkcapturedelegate.cpp decklinkcapturedelegate.h \
		glwidget.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o decklinkcapturedelegate.o decklinkcapturedelegate.cpp

DeckLinkAPIDispatch.o: include/DeckLinkAPIDispatch.cpp include/DeckLinkAPI.h \
		include/LinuxCOM.h \
		include/DeckLinkAPITypes.h \
		include/DeckLinkAPIModes.h \
		include/DeckLinkAPIDiscovery.h \
		include/DeckLinkAPIConfiguration.h \
		include/DeckLinkAPIDeckControl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DeckLinkAPIDispatch.o include/DeckLinkAPIDispatch.cpp

glwidget.o: glwidget.cpp glwidget.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o glwidget.o glwidget.cpp

window.o: window.cpp glwidget.h \
		window.h \
		decklinkcapturedelegate.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o window.o window.cpp

moc_glwidget.o: moc_glwidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_glwidget.o moc_glwidget.cpp

moc_window.o: moc_window.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_window.o moc_window.cpp

####### Install

install_target: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/var/lib/optiportal/hoang/NativeDecklinkCapture/./ || $(MKDIR) $(INSTALL_ROOT)/var/lib/optiportal/hoang/NativeDecklinkCapture/./ 
	-$(INSTALL_PROGRAM) "$(QMAKE_TARGET)" "$(INSTALL_ROOT)/var/lib/optiportal/hoang/NativeDecklinkCapture/$(QMAKE_TARGET)"
	-$(STRIP) "$(INSTALL_ROOT)/var/lib/optiportal/hoang/NativeDecklinkCapture/$(QMAKE_TARGET)"

uninstall_target:  FORCE
	-$(DEL_FILE) "$(INSTALL_ROOT)/var/lib/optiportal/hoang/NativeDecklinkCapture/$(QMAKE_TARGET)"
	-$(DEL_DIR) $(INSTALL_ROOT)/var/lib/optiportal/hoang/NativeDecklinkCapture/./ 


install:  install_target  FORCE

uninstall: uninstall_target   FORCE

FORCE:

