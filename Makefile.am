####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = kmag
kmag_SOURCES = kmagselrect.cpp kmagview.cpp kmagdoc.cpp kmag.cpp main.cpp 
kmag_LDADD   =  $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = main.cpp kmag.cpp kmag.h kmagdoc.cpp kmagdoc.h kmagview.cpp kmagview.h kmagui.rc kmag.desktop lo32-app-kmag.png lo16-app-kmag.png kmagselrect.cpp kmagselrect.h hi16-app-kmag.png 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/Accessibility/
	$(INSTALL_DATA) $(srcdir)/kmag.desktop $(kde_appsdir)/Applications/Accessibility/kmag.desktop
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-kmag.png $(kde_icondir)/locolor/32x32/apps/kmag.png
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/lo16-app-kmag.png $(kde_icondir)/locolor/16x16/apps/kmag.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/hi16-app-kmag.png $(kde_icondir)/hicolor/16x16/apps/kmag.png

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/Accessibility/kmag.desktop
	-rm -f $(kde_icondir)/locolor/32x32/apps/kmag.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/kmag.png
	-rm -f $(kde_icondir)/hicolor/16x16/apps/kmag.png

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
kmag_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/kmag
rc_DATA = kmagui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kmag.pot; \
	fi

