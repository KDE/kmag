bin_PROGRAMS = kmag

## INCLUDES were found outside kdevelop specific part

kmag_SOURCES = kmagzoomview.cpp kmagselrect.cpp kmag.cpp main.cpp 
kmag_LDADD   = -lkdeprint $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = main.cpp version.h kmag.cpp kmag.h kmagui.rc kmag.desktop lo32-app-kmag.png lo16-app-kmag.png kmagselrect.cpp kmagselrect.h hi16-app-kmag.png kmagzoomview.cpp kmagzoomview.h 

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/kmag.desktop

KDE_ICON = kmag 

kmagdir = $(kde_appsdir)/Applications
kmag_DATA = kmag.desktop

kmagiconsdir = $(kde_datadir)/kmag/icons
kmagicons_ICON = followmouse hidemouse window

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

KDE_OPTIONS = nofinal

# the library search path. 
kmag_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/kmag
rc_DATA = kmagui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kmag.pot; \
	fi

