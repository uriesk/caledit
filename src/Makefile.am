bin_PROGRAMS = caledit
caledit_SOURCES = caledit.cc calclass.cc eveclass.cc functions.cc

if ENABLE_GUI
caledit_SOURCES += gui-dialog.cc gui-mainwindow.cc
caledit_CPPFLAGS = $(GTKMM_CFLAGS)
caledit_LDFLAGS = $(GTKMM_LIBS)
endif

AM_CXXFLAGS= --std=c++11
